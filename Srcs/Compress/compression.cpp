#include <iostream>
#include <filesystem>
#include <string>
#include <windows.h>
#include "miniz.h"
#include <sstream>
#include <iomanip>
#include <clocale>
#include <fstream>

namespace fs = std::filesystem;

// 将 GBK 字符串转换为 UTF-8
std::string gbk_to_utf8(const std::string& gbk_str) {
    int wide_len = MultiByteToWideChar(936, 0, gbk_str.c_str(), -1, nullptr, 0);
    if (wide_len == 0) {
        // 处理错误
        return "";
    }
    std::wstring wide_str(wide_len, L'\0');
    MultiByteToWideChar(936, 0, gbk_str.c_str(), -1, &wide_str[0], wide_len);
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wide_str.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (utf8_len == 0) {
        // 处理错误
        return "";
    }
    std::string utf8_str(utf8_len, '\0');
    BOOL usedDefaultChar = FALSE;
    WideCharToMultiByte(CP_UTF8, 0, wide_str.c_str(), -1, &utf8_str[0], utf8_len, nullptr, &usedDefaultChar);

    // 去掉末尾的 \0 字符
    if (!utf8_str.empty() && utf8_str.back() == '\0') {
        utf8_str.pop_back();
    }

    return utf8_str;
}

// 将 UTF-8 字符串转换为 GBK
std::string utf8_to_gbk(const std::string& utf8_str) {
    int wide_len = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, nullptr, 0);
    if (wide_len == 0) {
        // 处理错误
        return "";
    }
    std::wstring wide_str(wide_len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wide_str[0], wide_len);
    int gbk_len = WideCharToMultiByte(936, 0, wide_str.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (gbk_len == 0) {
        // 处理错误
        return "";
    }
    std::string gbk_str(gbk_len, '\0');
    BOOL usedDefaultChar = FALSE;
    WideCharToMultiByte(936, 0, wide_str.c_str(), -1, &gbk_str[0], gbk_len, nullptr, &usedDefaultChar);

    // 去掉末尾的 \0 字符
    if (!gbk_str.empty() && gbk_str.back() == '\0') {
        gbk_str.pop_back();
    }

    return gbk_str;
}

// 检查系统的当前代码页是否是 UTF-8
bool is_utf8_code_page() {
    return GetACP() == CP_UTF8;
}

// 检查系统的当前代码页是否是 GBK
bool is_gbk_code_page() {
    return GetACP() == 936;  // 936 是 GBK 的代码页
}

// 将文件添加到zip
bool add_file_to_zip(mz_zip_archive& zip, const std::string& file_path, const std::string& zip_path) {//utf-8
    FILE* pFile;

    if (fopen_s(&pFile, file_path.c_str(), "rb") != 0 || pFile == nullptr) {
        std::cerr << "无法打开文件: " << file_path << std::endl;
        return false;
    }

    //file_size记录文件大小
    fseek(pFile, 0, SEEK_END);
    long file_size = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    //pFile_data的vector存储文件内容
    std::vector<char> pFile_data(file_size);
    fread(pFile_data.data(), 1, file_size, pFile);
    fclose(pFile);

    if (!mz_zip_writer_add_mem(&zip, zip_path.c_str(), pFile_data.data(), file_size, MZ_DEFAULT_COMPRESSION)) {
        std::cerr << "无法将文件添加到ZIP: " << zip_path << std::endl;
        return false;
    }
    return true;
}

// 压缩文件夹
bool compress_folder(const std::string& _folder_path, const std::string& _zip_file_path)
{
    //传入路径为utf-8编码；如果为GBK编码，就不需要utf8_zip_file_path，直接使用zip_file_path
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));

    std::string zip_file_path = _zip_file_path;

    bool utf8_env = is_utf8_code_page();  // 检查是否为UTF-8环境

    std::string utf8_zip_file_path = (utf8_env ? zip_file_path : gbk_to_utf8(zip_file_path));
    //由于miniz库是utf-8编码方式的,如果在GBK编码方式的系统上运行,需要将GBK的路径转为utf-8路径方能使用miniz库
    utf8_zip_file_path = zip_file_path;

    if (!mz_zip_writer_init_file(&zip, utf8_zip_file_path.c_str(), 0)) {
        std::cerr << "无法创建ZIP文件: " << zip_file_path << std::endl;
        return false;
    }
    std::cout << "成功创建ZIP文件: " << zip_file_path << std::endl;

    // 添加空文件夹
    fs::path folder_path = utf8_to_gbk(_folder_path);
    //递归遍历文件以实现子文件夹的添加
    for (const auto& entry : fs::recursive_directory_iterator(folder_path)) {
        //遍历到常规文件则添加文件至zip
        if (fs::is_regular_file(entry)) {
            std::string file_path = entry.path().string();//gbk
            std::string relative_path = fs::relative(file_path, folder_path).string();//gbk
            std::string zip_relative_path = utf8_env ? relative_path : gbk_to_utf8(relative_path);//utf-8
            std::cout << "正在压缩文件: " << relative_path << std::endl;
            if (!add_file_to_zip(zip, file_path, zip_relative_path)) {
                std::cerr << "添加文件失败: " << file_path << std::endl;
                mz_zip_writer_end(&zip);
                return false;
            }
        }
        //遍历到目录即子文件夹则添加空文件夹至zip
        else if (fs::is_directory(entry)) {
            // 获取相对路径
            std::string relative_dir_path = fs::relative(entry.path(), folder_path).string() + "/";
            std::string zip_relative_dir_path = utf8_env ? relative_dir_path : gbk_to_utf8(relative_dir_path);

            // 添加空文件夹到zip
            std::cout << "正在添加文件夹: " << relative_dir_path << std::endl;
            if (!mz_zip_writer_add_mem(&zip, (zip_relative_dir_path).c_str(), nullptr, 0, MZ_DEFAULT_COMPRESSION)) {
                std::cerr << "无法添加文件夹: " << relative_dir_path << std::endl;
                mz_zip_writer_end(&zip);
                return false;
            }
        }
    }

    //完成zip文件的写入过程
    if (!mz_zip_writer_finalize_archive(&zip)) {
        std::cerr << "无法完成ZIP文件写入" << std::endl;
        mz_zip_writer_end(&zip);
        return false;
    }

    //结束zip文件的写入过程
    mz_zip_writer_end(&zip);
    return true;
}


// 解压 ZIP 文件
bool decompress_zip(const std::string& _zip_file_path, const std::string& _extract_to) {
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));

    bool utf8_env = is_utf8_code_page();  // 检查是否为UTF-8环境

    std::string zip_file_path = _zip_file_path;
    fs::path extract_to = utf8_to_gbk(_extract_to);

    std::string km_file_path = utf8_to_gbk(zip_file_path);

    if (!fs::exists(extract_to)) {
        fs::create_directories(extract_to);
        std::cout << "成功创建目标文件夹: " << extract_to << std::endl;
    }

    //由于miniz库是utf-8编码方式的,如果在GBK编码方式的系统上运行,需要将GBK的路径转为utf-8路径方能使用miniz库
    std::string utf8_zip_file_path = (utf8_env ? zip_file_path : gbk_to_utf8(zip_file_path));
    utf8_zip_file_path = zip_file_path;
    if (!mz_zip_reader_init_file(&zip, utf8_zip_file_path.c_str(), 0)) {
        std::cerr << "无法打开 ZIP 文件: " << zip_file_path << std::endl;
        return false;
    }
    std::cout << "成功打开ZIP文件: " << zip_file_path << std::endl;
    // 获取 ZIP 文件中的文件数量
    int file_count = (int)mz_zip_reader_get_num_files(&zip);
    if (file_count < 0) {
        std::cerr << "无法获取 ZIP 文件中的文件数量" << std::endl;
        mz_zip_reader_end(&zip);
        return false;
    }
    std::cout << "ZIP 文件中的条目数量: " << file_count << std::endl;

    // 遍历 ZIP 文件中的每一个文件或文件夹
    for (int i = 0; i < file_count; i++) {
        mz_zip_archive_file_stat file_stat;//gbk

        // 读取 ZIP 文件中的文件状态
        if (!mz_zip_reader_file_stat(&zip, i, &file_stat)) {
            std::cerr << "无法读取 ZIP 文件信息，条目索引: " << i << std::endl;
            mz_zip_reader_end(&zip);
            return false;
        }

        // 获取原始文件名
        std::string original_filename = file_stat.m_filename;//gbk

        // 定义用于处理的文件名
        std::string processed_filename = utf8_env ? original_filename : utf8_to_gbk(original_filename);//gbk
        std::cout << "成功读取文件信息: " << processed_filename << std::endl;
        //if (file_stat.m_is_directory || processed_filename[processed_filename.size() - 2] == '\\') {
        //    std::cout << "条目名称: " << processed_filename << "， 是文件夹" << std::endl;
        //    processed_filename += "\\";
        //}

        //// 判断当前系统编码并进行转换
        //if (is_gbk_code_page()) {
        //    // 如果是 GBK，则将文件名从 UTF-8 转为 GBK
        //    processed_filename = utf8_to_gbk(original_filename);

        //    // 如果 processed_filename 是一个目录，确保它以反斜杠结尾
        //    if (file_stat.m_is_directory || processed_filename[processed_filename.size() - 2] == '\\') {
        //        std::cout << "条目名称: " << processed_filename << "， 是文件夹" << std::endl;
        //        processed_filename += "\\";
        //    }
        //}        

        // 处理文件夹或文件
        if (file_stat.m_is_directory || processed_filename[processed_filename.size() - 2] == '\\') {
            // 处理文件夹            
            fs::create_directories(extract_to / processed_filename); // 创建新目录
            std::cout << "成功创建文件夹: " << extract_to / processed_filename << std::endl;
        }
        else {
            // 处理文件
            std::string full_output_path = extract_to.string() + "/" + processed_filename;
            std::string new_full_output_path = utf8_env ? full_output_path : gbk_to_utf8(full_output_path);

            if (!mz_zip_reader_extract_to_file(&zip, i, new_full_output_path.c_str(), 0)) {
                std::cerr << "无法解压文件: " << new_full_output_path << std::endl;
                mz_zip_reader_end(&zip);
                return false;
            }
            std::cout << "解压文件成功: " << extract_to.string() + "/" + processed_filename << std::endl; // 输出解压成功的信息
        }
    }

    // 关闭 ZIP 文件
    mz_zip_reader_end(&zip);

    return true;
}

