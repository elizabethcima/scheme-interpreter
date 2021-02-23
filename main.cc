#include <cassert>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

#include "cell.h"
#include "eval.h"

std::shared_ptr<Cell> BuildCell(std::sregex_iterator& input_list) {
  // check if end of bracket or end of sentence is reached
  if (input_list == std::sregex_iterator()) {
    // to do: give out an error here
    return nullptr;
  } else if ((*input_list).str().compare(")") == 0) {
    return nullptr;
  } else if ((*input_list).str().compare("(") == 0) {
    // check if tree needs to go down a level with left bracket
    // special case: the first left bracket doesn't require the architecture to
    // have an additonal layer.
    return std::make_shared<Cell>(
	BuildCell(++input_list), BuildCell(++input_list));
  } else {
    // differenciate numbers
    char* p;

    strtol((*input_list).str().c_str(), &p, 10);
    if ((*p) == 0) {

      // check if this is a float number
      if ((*input_list).str().find(".") != std::string::npos) {
	return std::make_shared<Cell>(
	    std::make_shared<FloatCell>(std::stof((*input_list).str())),
	    BuildCell(++input_list));
      } else {
	return std::make_shared<Cell>(
	    std::make_shared<IntCell>(std::stoi((*input_list).str())),
	    BuildCell(++input_list));
      }
    } else {
      return std::make_shared<Cell>(
	  std::make_shared<SymbolCell>((*input_list).str()),
	  BuildCell(++input_list));
    }
  }
}

bool left_bracket_equals_right_bracket(std::sregex_iterator input_list) {

  int left_bracket = 0;
  int right_bracket = 0;
  while (input_list != std::sregex_iterator()) {
    if ((*input_list).str() == "(") {
      left_bracket++;
    } else if ((*input_list).str() == ")") {
      right_bracket++;
    }
    ++input_list;
  }

  return (left_bracket == right_bracket);
}

std::shared_ptr<Cell> parse(const std::string& code) {
  assert(code != "quit");

  // seperate user input string into symbol and number objects

  // matches delimiters or consecutive non-delimiters
  std::regex delimiter_and_object(
      "([\\+\\-\\*\\/\\(\\)]|[^\\+\\-\\*\\/\\s\\(\\)]+)");

  auto words_begin =
      std::sregex_iterator(code.begin(), code.end(), delimiter_and_object);
  auto words_end = std::sregex_iterator();

  if (!left_bracket_equals_right_bracket(words_begin)) {
    std::cout << "unmatched left and right bracket" << '\n';
    return nullptr;
  }

  return BuildCell(words_begin)->car();
}

int main(int argc, char** argv) {

  // initialize Env
  Environment env;
  auto global_map = std::make_shared<
      std::unordered_map<std::string, std::shared_ptr<Cell>>>();
  env.push_back(global_map);

  std::string code;
  int i = 0;

  if (argc > 2) {
    std::cout << "going into test & eval" << '\n';
    std::ifstream test_input(argv[1]);
    std::ifstream test_output(argv[2]);
    if (test_output.is_open() && test_input.is_open()) {
      std::string input_line;
      std::string result_line;
      int i = 0;
      while (std::getline(test_input, input_line)) {
	if (std::getline(test_output, result_line)) {
	  std::cout << input_line << '\n';
	  auto cell = parse(input_line);
	  if (cell != nullptr) {
	    auto ret = eval(&env, cell);
	    if (result_line.compare(ret->quote()) != 0) {
	      std::cout << "[" << i + 1 << "]: "
			<< "test result should be:" << result_line << '\n';
	      std::cout << "[" << i + 1 << "]: "
			<< "test result is:" << ret->quote() << '\n';
	    }
	  }
	} else {
	  std::cout << "reaching end of test result file" << '\n';
	}
	++i;
      }
      std::cout << "pass" << '\n';
      test_input.close();
      test_output.close();
    }

  } else {
    while (true) {
      std::getline(std::cin, code);
      if (code == "quit") {
	break;
      }
      auto cell = parse(code);
      if (cell != nullptr) {
	auto ret = eval(&env, cell);
	if (ret != nullptr) {
	  std::cout << "[" << i + 1 << "]: " << ret->quote() << std::endl;
	} else {
	  std::cout << "[" << i + 1 << "]" << std::endl;
	}
	++i;
      }
    }
    std::cout << "terminate" << std::endl;
  }
  return 0;
}
