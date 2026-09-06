#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <ranges>
#include <random>

constexpr std::string RED = "\033[31m";
constexpr std::string GREEN = "\033[32m";
constexpr std::string BLUE = "\033[34m";
constexpr std::string RESET = "\033[0m";

const std::map<std::string, std::string> COLOUR_MAP = {
   {"red", RED},
   {"green", GREEN},
   {"blue", BLUE}
};

constexpr std::string WHITESPACE = " \t\n";

std::string strip_text_after_comment(std::string &&line) {
   auto first_char = line.find("//");
   if (first_char == std::string::npos) {
      return std::move(line);
   }

   line.erase(first_char);
   return std::move(line);
}

std::string strip_text_after_comment(const std::string &line) = delete;

std::vector<std::pair<std::string, std::vector<std::string> > > get_commands_and_alt_text(std::ifstream &file) {
   std::vector<std::pair<std::string, std::vector<std::string> > > result_vec{};
   std::string line;
   std::streampos prev_line_pos = file.tellg();

   while (std::getline(file, line)) {
      line = strip_text_after_comment(std::move(line));

      auto expected_dash_loc = line.find_first_not_of(WHITESPACE);
      if (expected_dash_loc == std::string::npos || line[expected_dash_loc] != '-') continue;

      prev_line_pos = file.tellg();

      auto command_start_loc = line.find_first_not_of(WHITESPACE, expected_dash_loc + 1);
      if (command_start_loc == std::string::npos) continue;

      std::string command = line.substr(command_start_loc);
      std::vector<std::string> description{};

      while (std::getline(file, line)) {
         line = strip_text_after_comment(std::move(line));

         auto non_whitespace_loc = line.find_first_not_of(WHITESPACE);
         if (non_whitespace_loc != std::string::npos && line[non_whitespace_loc] == '-') {
            file.seekg(prev_line_pos);
            break;
         }
         if (non_whitespace_loc == std::string::npos) continue;

         description.push_back(line);
      }

      result_vec.emplace_back(command, description);
   }

   return result_vec;
}

int find_and_replace_all(std::string &line, const std::string &to_find, const std::string &to_replace_with) {
   int elements_replaced{};
   for (size_t loc = line.find(to_find); loc != std::string::npos; loc = line.find(to_find, loc)) {
      line.replace(loc, to_find.size(), to_replace_with);
      ++elements_replaced;
   }
   return elements_replaced;
}

void print_chosen_command_and_description(std::string &command, std::vector<std::string> &description) {
   std::cout << std::format("{}{}{}", GREEN, command, RESET) << std::endl;

   for (size_t i{}; i < description.size(); i++) {
      auto &line = description[i];
      // Very inefficient, but easy way to colour any instances of the command in the description.
      for (size_t comm_loc = line.find(command); comm_loc != std::string::npos;
           comm_loc = line.find(command, comm_loc)) {
         line.insert(comm_loc, GREEN);
         comm_loc += GREEN.size() + command.size();
         line.insert(comm_loc, RESET);
         comm_loc += RESET.size();
      }

      for (auto &[colour, colour_ansi_code]: COLOUR_MAP) {
         const auto opens_replaced = find_and_replace_all(line, std::format("<{}>", colour), colour_ansi_code);
         const auto closes_replaced = find_and_replace_all(line, std::format("</{}>", colour), RESET);

         if (opens_replaced != closes_replaced) {
            std::cerr << RED
                  << std::format("Error number of <{}> tags != number of </{}> tags: {} != {}",
                                 colour, colour, opens_replaced, closes_replaced)
                  << RESET;
         }
      }

      std::cout << "   " << line << std::endl;
   }
}

int main(int argc, char **argv) {
   if (argc <= 1) {
      std::cerr << "No commands file passed as argument!" << std::endl;
      return 1;
   }
   if (argc >= 3) {
      std::cerr << std::format("Too many commands passed to 'commando': {}", argc) << std::endl;
      return 1;
   }

   std::filesystem::path commands_file = argv[1];
   std::ifstream file(commands_file);

   auto result_vec = get_commands_and_alt_text(file);

   std::mt19937 gen{std::random_device{}()};
   std::uniform_int_distribution dist(0, static_cast<int>(result_vec.size()) - 1);

   auto random_num = dist(gen);

   auto &[command, description] = result_vec[random_num];

   print_chosen_command_and_description(command, description);

   return 0;
}
