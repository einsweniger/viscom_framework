//
// Created by bone on 25.04.18.
//

#ifndef VISCOMFRAMEWORK_FILESELECT_H
#define VISCOMFRAMEWORK_FILESELECT_H

#include <experimental/filesystem>
#include <functional>
#include <unordered_map>
#include <vector>

#include <imgui.h>


namespace minuseins::gui {
namespace fs = std::experimental::filesystem;
struct FileSelect {
  ImGuiTextFilter filter;

  std::string name;
  std::function<bool(fs::path)> callback;
  std::vector<fs::path> basepaths_;
  std::unordered_map<std::string, std::vector<fs::path>> pathContents;
  std::unordered_map<std::string, fs::path> currentPath;
  std::vector<fs::path> paths{};

  void draw(bool *p_open);

  FileSelect(const std::string &name, std::vector<std::string> pathstrs,
             const std::function<bool(fs::path)> callback,
             std::string basepath_suffix = "");

  std::vector<fs::path> scan(fs::path folder);

  void drawMenu(fs::path &base);
};

}  // namespace minuseins::gui

#endif  // VISCOMFRAMEWORK_FILESELECT_H
