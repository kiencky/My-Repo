//=================================================================
// config_loader.h
// Note: Config parser interface for the MiniScheduler daemon.
//=================================================================

#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include "config.h"

/// @brief  Parse config file into config.
/// @param  file_path  Path to the config file.
/// @param  config     Output struct.
/// @return 0 : Success (or file not found, defaults applied).
///        -1 : Error reading file.
/// @note   Missing keys are filled with defaults.
///         If the file is not found, defaults are used and 0 is returned.
int config_load(const char *file_path, minisched_config_t *config);

#endif  // CONFIG_LOADER_H