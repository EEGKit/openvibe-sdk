OV_SET_CUSTOM_DOCUMENTATION("ebml" "EBML parser and formater library" "src" "include")
# Add all the subdirs as projects of the named branch
OV_ADD_PROJECTS("MODULES")

# Sort target into directories for better visualization in IDE
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
SET(MODULES_FOLDER Modules)