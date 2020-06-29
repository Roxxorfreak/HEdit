// Copyright (c) 2020 Roxxorfreak

#include "headers.hpp"

/**
 * Creates a new file instance initializing the buffer for the cache (if the
 * file uses caching) and assigns a file name.
 * @param file_name The name of the file to manage.
 * @param use_cache true to use caching, false to do not.
 */
TConfigFile::TConfigFile(const char* file_name) : TFile(file_name, true)
{
}
