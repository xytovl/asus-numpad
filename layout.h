#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

class Layout
{
  public:
    Layout(std::vector<int> keys, uint8_t num_cols, double top_offset)
        : top_offset{top_offset}, num_cols{num_cols}, keys{keys}
    {
        num_rows = keys.size() / num_cols;
    }
    int operator()(double x, double y) const
    {
        int32_t col = x * num_cols;
        int32_t row = (y - top_offset) * num_rows;
        if (col >= num_cols or row < 0 or row >= num_rows)
            return 0;
        return keys[col + row * num_cols];
    }

    auto begin() const
    {
        return keys.begin();
    }
    auto end() const
    {
        return keys.end();
    }

  private:
    double top_offset;
    uint8_t num_cols;
    uint8_t num_rows;
    std::vector<int> keys;
};

Layout parse_layout(const std::filesystem::path &path, std::string model = "");
