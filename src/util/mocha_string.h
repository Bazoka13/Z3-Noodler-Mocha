#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <string_view>
#include <unordered_map>
#include "util/buffer.h"
#include "util/rational.h"
#include "util/zstring.h"
#include "util/uint_set.h"
#include "ast/rewriter/seq_rewriter.h"
#include "ast/arith_decl_plugin.h"
#include "ast/array_decl_plugin.h"
#include "ast/ast_pp.h"
#include "ast/ast_ll_pp.h"
#include "ast/ast_util.h"
#include "ast/well_sorted.h"
#include "ast/rewriter/var_subst.h"
#include "ast/rewriter/expr_safe_replace.h"
#include "math/automata/automaton.h"
#include "math/automata/symbolic_automata_def.h"

bool subOf(zstring const& s, zstring const& t);
int idxOf(zstring const& s, zstring const& t,unsigned offset);
bool mmatch(std::string &s,std::string &t);