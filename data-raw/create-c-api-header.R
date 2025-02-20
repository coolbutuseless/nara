
library(stringr)
library(glue)
library(purrr)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Include the standard headers 
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
includes <- r"(
#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>


)" 


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# 'glue' template for each function
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
func_template <- r"(
static inline {{decl}} {
  static {{type}} (*fun){{args}} = NULL;

  if (fun == NULL) {
    fun = ({{type}} (*){{args}}) R_GetCCallable("nara", "{{name}}");
  }

  {{creturn}}fun({{nms}});
}
)"

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Tidy the declarations and extract the components
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
decl_to_func <- function(decl) {
  decl <- decl |>
    str_replace_all("\\s+", " ") |>
    str_replace_all(";", "") |> 
    str_trim()
  
  # return_type, name, arg_list
  bits <- stringr::str_split_fixed(decl, "\\(", 2)
  type_name <- bits[[1]] |> 
    str_split(" ") |> 
    el()
  type <- type_name[[1]]
  name <- type_name[[2]]
  
  args <- bits[[2]] |> 
    str_trim()
  args <- paste0("(", args)
  
  arg_types_names <- args |>
    str_replace("^\\(", "") |>
    str_replace("\\)$", "") |>
    str_split(",") |> 
    el() |> 
    str_trim() |> 
    str_split("\\s+")
  
  arg_types <- map_chr(arg_types_names, \(v) paste(head(v, -1), collapse = " "))
  arg_names <- map_chr(arg_types_names, \(v) tail(v, 1))

  nms <- arg_names |> 
    str_replace_all("\\*", "") |>
    paste(collapse = ", ")
  
  creturn <- ifelse(type == 'void', "", "return ")
  
  glue::glue(func_template, .open = "{{", .close = "}}")
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Typedefs for export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
typedefs <- r"(


typedef enum {
  RESPECT_ALPHA = 1,
  IGNORE_ALPHA  = 2
} draw_mode_t;


)"


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Read all the declarations from a header file
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
header_files <- c(
  "./src/nr-core.h",
  "./src/nr-draw.h",
  "./src/nr-blit.h"
)

# header_file <- "./src/nr-draw.h"

header_to_funcs <- function(header_file) {
  decls <- readLines(header_file)
  decls <- decls[decls != '']
  
  map_chr(decls, decl_to_func)
}

funcs <- map(header_files, header_to_funcs) |> purrr::flatten_chr()
funcs <- paste(funcs, collapse = "\n\n\n")


nara_h <- c(includes, typedefs, funcs)
writeLines(nara_h, "./inst/include/private/nara.h")


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# header to init-decls.h
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
decls <- map(header_files, function(f) {
  decls <- readLines(f)
  decls <- decls[decls != '']
  decls
}) |> flatten_chr()

decls <- paste("extern", decls)
decls <- c(typedefs, decls)
writeLines(decls, "./src/init-decls.h")


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Register callables
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# R_RegisterCCallable("nara", "nr_line"   , (DL_FUNC) &nr_line);

decl_to_name <- function(decl) {
  decl <- decl |>
    str_replace_all("\\s+", " ") |>
    str_replace_all(";", "") |> 
    str_trim()
  
  # return_type, name, arg_list
  bits <- stringr::str_split_fixed(decl, "\\(", 2)
  type_name <- bits[[1]] |> 
    str_split(" ") |> 
    el()
  type_name[[2]]
}

decl_to_register <- function(decl) {
  nm <- decl_to_name(decl)
  sprintf('R_RegisterCCallable("nara", "%s"   , (DL_FUNC) &%s);', nm, nm)
}

decls <- map(header_files, function(f) {
  decls <- readLines(f)
  decls <- decls[decls != '']
  decls
}) |> flatten_chr()

register_callables <- map_chr(decls, decl_to_register)

writeLines(register_callables, "./src/init-register.h")






