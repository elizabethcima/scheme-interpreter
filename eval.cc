#include "eval.h"
#include <cassert>
#include <iostream>

std::shared_ptr<Cell> eval(
    Environment* env, const std::shared_ptr<Cell>& cell) {
  if (cell->get_type() == CellType::Num) {
    return std::make_shared<IntCell>(
	dynamic_cast<const IntCell*>(cell.get())->get_value());
  } else if (cell->get_type() == CellType::Symbol) {
    return symbol_eval(env, cell);
  } else {
    auto function = cell->car()->quote();
    if (function == "define") {
      return define_eval(env, cell);
    } else if (function == "+") {
      return operations_eval(env, cell, [](int x, int y) { return x + y; });
    } else if (function == "-") {
      return operations_eval(env, cell, [](int x, int y) { return x - y; });
    } else if (function == "*") {
      return operations_eval(env, cell, [](int x, int y) { return x * y; });
    } else if (function == "/") {
      return operations_eval(env, cell, [](int x, int y) { return x / y; });
    } else if (function == "<") {
      assert(top_level_length(cell) == 3);
      return operations_eval(
	  env, cell, [](int x, int y) { return int(x < y); });
    } else if (function == "if") {
      return if_eval(env, cell);
    } else if (function == "let") {
      return let_eval(env, cell);
    } else if (function == "lambda") {
      return lambda_eval(env, cell);
    } else if (function == "cons") {
      return cons_eval(env, cell);
    } else if (function == "car") {
      return car_eval(env, cell);
    } else if (function == "cdr") {
      return cdr_eval(env, cell);
    } else if (function == "list") {
      return list_eval(env, cell->cdr());
    } else {
      return function_eval(env, cell);
    }
  }
  return nullptr;
}

std::shared_ptr<Cell> symbol_eval(
    Environment* env, const std::shared_ptr<Cell>& cell) {
  for (auto it = (*env).rbegin(); it != (*env).rend(); ++it) {
    auto kv = (*it)->find(cell->quote());
    if (kv != (*it)->end()) {
      // retrival succeeded, judge if this is a function
      if (kv->second->get_type() == CellType::Procedure) {
	// this is not a varaible but a function
	return kv->second;
      } else {
	return kv->second;
      }
    }
  }

  std::cout << "variable" << cell->quote() << "undefinied" << std::endl;
  return nullptr;
}

std::shared_ptr<Cell> function_eval(
    Environment* env, const std::shared_ptr<Cell>& cell) {
  auto function_cell = eval(env, cell->car());

  if (function_cell != nullptr) {
    // retrival succeeded, judge if this is a function
    assert(function_cell->get_type() == CellType::Procedure);
    auto procedure_cell = dynamic_cast<ProcedureCell*>(function_cell.get());

    // retrieving function scope variable map
    auto func_env = procedure_cell->getEnvironment();

    // constructing a new map of variables that only exist in this function
    auto func_variable_map = std::make_shared<
	std::unordered_map<std::string, std::shared_ptr<Cell>>>();

    // iterate through function argument to give value to arguments
    auto current_func_cell = procedure_cell->getParam();
    auto current_cell = cell->cdr();

    assert(
	top_level_length(current_func_cell) == top_level_length(current_cell));
    while (current_func_cell != nullptr) {
      auto key = current_func_cell->car()->quote();
      auto val = eval(env, current_cell->car());
      (*func_variable_map)[key] = val;
      current_func_cell = current_func_cell->cdr();
      current_cell = current_cell->cdr();
    }
    // put the new variable map to the back of the env list.
    func_env.push_back(func_variable_map);
    // evaluate function body
    auto result = eval(&func_env, procedure_cell->getOperation());
    return result;
  } else {
    return nullptr;
  }
}

std::shared_ptr<Cell> operations_eval(
    Environment* env, const std::shared_ptr<Cell>& cell, int func(int, int)) {
  // retrieve number value of the first cell
  assert(top_level_length(cell) > 2);
  auto first_cell = eval(env, cell->cdr()->car());
  auto first_int_cell = dynamic_cast<IntCell*>(first_cell.get());
  assert(first_int_cell != nullptr);
  int result = first_int_cell->get_value();

  // iteratively apply lamda function to realize the basic four operations
  for (auto current_cell = cell->cdr()->cdr(); current_cell != nullptr;
       current_cell = current_cell->cdr()) {
    auto first_cell = eval(env, current_cell->car());
    auto first_int_cell = dynamic_cast<const IntCell*>(first_cell.get());
    assert(first_int_cell != nullptr);
    int value = first_int_cell->get_value();
    result = func(result, value);
  }
  return std::make_shared<IntCell>(result);
}

std::shared_ptr<Cell> if_eval(
    Environment* env, const std::shared_ptr<Cell>& cell) {
  assert(top_level_length(cell) == 4);
  // getting true of false result
  auto pred = eval(env, cell->cdr()->car());
  auto pred_int = dynamic_cast<IntCell*>(pred.get());
  assert(pred_int != nullptr);
  if (pred_int->get_value() == 1) {
    return eval(env, cell->cdr()->cdr()->car());
  } else {
    return eval(env, cell->cdr()->cdr()->cdr()->car());
  }
}

std::shared_ptr<Cell> define_eval(
    Environment* env, const std::shared_ptr<Cell>& cell) {
  // check if this is defining a variable or function
  assert(top_level_length(cell) > 2);
  auto define_object_cell =
      dynamic_cast<const SymbolCell*>(cell->cdr()->car().get());
  if (define_object_cell != nullptr) {
    // going into variable definition logic
    assert(cell->cdr()->cdr()->car() != nullptr);
    auto val = eval(env, cell->cdr()->cdr()->car());
    auto key = cell->cdr()->car()->quote();

    insert_to_env(env, key, std::move(val));
  } else {
    // going into function definition logic
    assert(top_level_length(cell) == 3);
    auto key = cell->cdr()->car()->car()->quote();
    auto param = cell->cdr()->car()->cdr();
    auto operation = cell->cdr()->cdr()->car();
    auto function_cell =
	std::make_shared<ProcedureCell>(param, operation, *env);

    insert_to_env(env, key, function_cell);
  }

  return nullptr;
}

std::shared_ptr<Cell> lambda_eval(
    Environment* env, const std::shared_ptr<Cell>& cell) {
  auto param = cell->cdr()->car();
  auto operation = cell->cdr()->cdr()->car();
  auto function_cell = std::make_shared<ProcedureCell>(param, operation, *env);
  return function_cell;
}

std::shared_ptr<Cell> cons_eval(
    Environment* env, const std::shared_ptr<Cell>& cell) {
  auto car_cell = eval(env, cell->cdr()->car());
  auto cdr_cell = eval(env, cell->cdr()->cdr()->car());

  auto return_cell = std::make_shared<Cell>(car_cell, cdr_cell);
  return return_cell;
}

std::shared_ptr<Cell> car_eval(
    Environment* env, const std::shared_ptr<Cell>& cell) {
  auto cons = eval(env, cell->cdr()->car());
  auto car_cell = cons->car();
  return car_cell;
}

std::shared_ptr<Cell> cdr_eval(
    Environment* env, const std::shared_ptr<Cell>& cell) {
  auto cons = eval(env, cell->cdr()->car());
  auto cdr_cell = cons->cdr();
  return cdr_cell;
}

std::shared_ptr<Cell> list_eval(
    Environment* env, const std::shared_ptr<Cell>& cell) {
  if (cell->cdr() == nullptr) {
    return std::make_shared<Cell>(eval(env, cell->car()), nullptr);
  }
  return std::make_shared<Cell>(
      eval(env, cell->car()), list_eval(env, cell->cdr()));
}

std::shared_ptr<Cell> let_eval(
    Environment* env, const std::shared_ptr<Cell>& cell) {
  // constructing a new map of variables that only exist in this function
  auto let_variable_map = std::make_shared<
      std::unordered_map<std::string, std::shared_ptr<Cell>>>();

  // iterate through let variable cluse to give value to arguments
  assert(top_level_length(cell) == 3);
  auto current_var = cell->cdr()->car();
  while (current_var != nullptr) {
    assert(current_var->car() != nullptr);
    auto current_var_name = current_var->car()->car()->quote();
    auto current_var_body = eval(env, current_var->car()->cdr()->car());
    (*let_variable_map)[current_var_name] = current_var_body;
    current_var = current_var->cdr();
  }
  (*env).push_back(let_variable_map);

  // evaluate let body
  assert(cell->cdr()->cdr()->car() != nullptr);

  auto result = eval(env, cell->cdr()->cdr()->car());
  // delete the temporary map
  (*env).pop_back();

  return result;
}

void insert_to_env(
    Environment* env, std::string key, std::shared_ptr<Cell> val) {
  if (!(*env).empty()) {
    (*(*env).front())[key] = std::move(val);
  } else {
    auto variable_map = std::make_shared<
	std::unordered_map<std::string, std::shared_ptr<Cell>>>();
    (*variable_map)[key] = std::move(val);
    (*env).push_back(variable_map);
  }
}
