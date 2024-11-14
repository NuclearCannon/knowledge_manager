#pragma once
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

// 压缩文件夹
bool compress_folder(const std::string& _folder_path, const std::string& _zip_file_path);
// 解压缩文件夹
bool decompress_zip(const std::string& zip_file_path, const std::string& _extract_to);