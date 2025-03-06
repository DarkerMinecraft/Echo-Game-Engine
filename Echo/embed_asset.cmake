# embed_asset.cmake
# This script expects two variables:
#   INPUT_FILE  - the asset file to embed
#   OUTPUT_FILE - the header file to generate

if(NOT DEFINED INPUT_FILE)
  message(FATAL_ERROR "INPUT_FILE not defined")
endif()
if(NOT DEFINED OUTPUT_FILE)
  message(FATAL_ERROR "OUTPUT_FILE not defined")
endif()

# Read the asset file into a variable.
file(READ "${INPUT_FILE}" ASSET_CONTENT)

# Create a variable name based on the asset file name (without extension)
get_filename_component(FILE_NAME "${INPUT_FILE}" NAME_WE)
# Replace any non-alphanumeric characters with underscores
string(REGEX REPLACE "[^a-zA-Z0-9_]" "_" VAR_NAME "${FILE_NAME}")

# Build the header content.
# Wrap the asset content in a wide raw string literal (LR"(...)" )
set(HEADER_CONTENT "// Auto-generated header for asset: ${INPUT_FILE}\n")
set(HEADER_CONTENT "${HEADER_CONTENT}namespace Echo {\n")
set(HEADER_CONTENT "${HEADER_CONTENT}    const wchar_t* ${VAR_NAME} = LR\"(\n${ASSET_CONTENT}\n)\";\n")
set(HEADER_CONTENT "${HEADER_CONTENT}}\n")

# Write the generated header to OUTPUT_FILE.
file(WRITE "${OUTPUT_FILE}" "${HEADER_CONTENT}")
