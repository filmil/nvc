//
//  Copyright (C) 2011-2021  Nick Gasson
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "tree.h"
#include "util.h"
#include "object.h"
#include "common.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static const imask_t has_map[T_LAST_TREE_KIND] = {
   // T_ENTITY
   (I_IDENT | I_PORTS | I_GENERICS | I_CONTEXT | I_DECLS | I_STMTS),

   // T_ARCH
   (I_IDENT | I_IDENT2 | I_DECLS | I_STMTS | I_CONTEXT | I_PRIMARY),

   // T_PORT_DECL
   (I_IDENT | I_VALUE | I_TYPE | I_SUBKIND | I_CLASS | I_FLAGS),

   // T_FCALL
   (I_IDENT | I_PARAMS | I_TYPE | I_REF | I_FLAGS),

   // T_LITERAL
   (I_SUBKIND | I_TYPE | I_IVAL | I_DVAL | I_CHARS | I_FLAGS | I_IDENT | I_REF),

   // T_SIGNAL_DECL
   (I_IDENT | I_VALUE | I_TYPE | I_FLAGS | I_IDENT2),

   // T_VAR_DECL
   (I_IDENT | I_VALUE | I_TYPE | I_FLAGS | I_IDENT2),

   // T_PROCESS
   (I_IDENT | I_DECLS | I_STMTS | I_TRIGGERS | I_FLAGS),

   // T_REF
   (I_IDENT | I_TYPE | I_REF | I_FLAGS),

   // T_WAIT
   (I_IDENT | I_VALUE | I_DELAY | I_TRIGGERS | I_FLAGS),

   // T_TYPE_DECL
   (I_IDENT | I_TYPE),

   // T_VAR_ASSIGN
   (I_IDENT | I_VALUE | I_TARGET),

   // T_PACKAGE
   (I_IDENT | I_DECLS | I_CONTEXT),

   // T_SIGNAL_ASSIGN
   (I_IDENT | I_TARGET | I_WAVES | I_REJECT),

   // T_QUALIFIED
   (I_IDENT | I_VALUE | I_TYPE),

   // T_ENUM_LIT
   (I_IDENT | I_TYPE | I_POS),

   // T_CONST_DECL
   (I_IDENT | I_VALUE | I_TYPE | I_FLAGS | I_IDENT2),

   // T_FUNC_DECL
   (I_IDENT | I_PORTS | I_TYPE | I_FLAGS | I_IDENT2 | I_SUBKIND),

   // T_ELAB
   (I_IDENT | I_DECLS | I_STMTS | I_CONTEXT | I_EOPT),

   // T_AGGREGATE
   (I_TYPE | I_ASSOCS | I_FLAGS),

   // T_ASSERT
   (I_IDENT | I_VALUE | I_SEVERITY | I_MESSAGE),

   // T_ATTR_REF
   (I_NAME | I_VALUE | I_IDENT | I_PARAMS | I_TYPE | I_SUBKIND),

   // T_ARRAY_REF
   (I_VALUE | I_PARAMS | I_TYPE | I_FLAGS),

   // T_ARRAY_SLICE
   (I_VALUE | I_TYPE | I_RANGES),

   // T_INSTANCE
   (I_IDENT | I_IDENT2 | I_PARAMS | I_GENMAPS | I_REF | I_CLASS | I_SPEC),

   // T_IF
   (I_IDENT | I_VALUE | I_STMTS | I_ELSES),

   // T_NULL
   (I_IDENT),

   // T_PACK_BODY
   (I_IDENT | I_DECLS | I_CONTEXT | I_PRIMARY),

   // T_FUNC_BODY
   (I_IDENT | I_DECLS | I_STMTS | I_PORTS | I_TYPE | I_FLAGS
    | I_IDENT2 | I_SUBKIND),

   // T_RETURN
   (I_IDENT | I_VALUE),

   // T_CASSIGN
   (I_IDENT | I_TARGET | I_CONDS | I_GUARD),

   // T_WHILE
   (I_IDENT | I_VALUE | I_STMTS),

   // T_WAVEFORM
   (I_VALUE | I_DELAY),

   // T_ALIAS
   (I_IDENT | I_VALUE | I_TYPE | I_IDENT2),

   // T_FOR
   (I_IDENT | I_STMTS | I_RANGES | I_DECLS),

   // T_ATTR_DECL
   (I_IDENT | I_TYPE),

   // T_ATTR_SPEC
   (I_IDENT | I_VALUE | I_IDENT2 | I_CLASS | I_REF),

   // T_PROC_DECL
   (I_IDENT | I_PORTS | I_TYPE | I_FLAGS | I_IDENT2 | I_SUBKIND),

   // T_PROC_BODY
   (I_IDENT | I_DECLS | I_STMTS | I_PORTS | I_TYPE | I_FLAGS
    | I_IDENT2 | I_SUBKIND),

   // T_EXIT
   (I_IDENT | I_VALUE | I_IDENT2),

   // T_PCALL
   (I_IDENT | I_IDENT2 | I_PARAMS | I_REF),

   // T_CASE
   (I_IDENT | I_VALUE | I_ASSOCS),

   // T_BLOCK
   (I_IDENT | I_DECLS | I_STMTS | I_PORTS | I_GENERICS | I_PARAMS | I_GENMAPS),

   // T_COND
   (I_VALUE | I_WAVES | I_REJECT),

   // T_TYPE_CONV
   (I_VALUE | I_TYPE | I_FLAGS),

   // T_SELECT
   (I_IDENT | I_VALUE | I_ASSOCS | I_GUARD),

   // T_COMPONENT
   (I_IDENT | I_PORTS | I_GENERICS),

   // T_IF_GENERATE
   (I_IDENT | I_VALUE | I_DECLS | I_STMTS),

   // T_FOR_GENERATE
   (I_IDENT | I_DECLS | I_STMTS | I_RANGES),

   // T_FILE_DECL
   (I_IDENT | I_VALUE | I_TYPE | I_FILE_MODE | I_IDENT2),

   // T_OPEN
   (I_TYPE),

   // T_FIELD_DECL
   (I_IDENT | I_TYPE | I_POS),

   // T_RECORD_REF
   (I_IDENT | I_VALUE | I_TYPE | I_REF),

   // T_ALL
   (I_VALUE | I_TYPE),

   // T_NEW
   (I_VALUE | I_TYPE),

   // T_CASSERT
   (I_IDENT | I_VALUE | I_SEVERITY | I_MESSAGE | I_FLAGS),

   // T_CPCALL
   (I_IDENT | I_IDENT2 | I_PARAMS | I_REF),

   // T_UNIT_DECL
   (I_IDENT | I_VALUE | I_TYPE),

   // T_NEXT
   (I_IDENT | I_VALUE | I_IDENT2),

   // T_GENVAR
   (I_IDENT | I_TYPE | I_FLAGS),

   // T_PARAM
   (I_VALUE | I_POS | I_SUBKIND | I_NAME),

   // T_ASSOC
   (I_VALUE | I_POS | I_NAME | I_RANGES | I_SUBKIND),

   // T_USE
   (I_IDENT | I_IDENT2 | I_REF),

   // T_HIER
   (I_IDENT | I_SUBKIND | I_IDENT2),

   // T_SPEC
   (I_IDENT | I_IDENT2 | I_VALUE | I_REF | I_DECLS),

   // T_BINDING
   (I_PARAMS | I_GENMAPS | I_IDENT | I_IDENT2 | I_CLASS | I_REF),

   // T_LIBRARY
   (I_IDENT | I_IDENT2),

   // T_DESIGN_UNIT
   (I_CONTEXT),

   // T_CONFIGURATION
   (I_IDENT | I_IDENT2 | I_DECLS | I_PRIMARY),

   // T_PROT_BODY
   (I_IDENT | I_TYPE | I_DECLS),

   // T_CONTEXT
   (I_CONTEXT | I_IDENT),

   // T_CTXREF
   (I_IDENT | I_REF),

   // T_CONSTRAINT
   (I_SUBKIND | I_RANGES),

   // T_BLOCK_CONFIG
   (I_DECLS | I_IDENT | I_VALUE | I_RANGES | I_REF),

   // T_PROT_FCALL
   (I_IDENT | I_PARAMS | I_TYPE | I_REF | I_FLAGS | I_NAME),

   // T_PROT_PCALL
   (I_IDENT | I_IDENT2 | I_PARAMS | I_REF | I_NAME),

   // T_RANGE
   (I_SUBKIND | I_VALUE | I_LEFT | I_RIGHT | I_TYPE),

   // T_IMPLICIT_SIGNAL
   (I_IDENT | I_TYPE | I_SUBKIND | I_VALUE | I_FLAGS),

   // T_DISCONNECT
   (I_IDENT | I_REF | I_TYPE | I_DELAY),

   // T_GROUP_TEMPLATE
   (I_IDENT),

   // T_GROUP
   (I_IDENT | I_REF),

   // T_SUBTYPE_DECL
   (I_IDENT | I_TYPE),
};

static const char *kind_text_map[T_LAST_TREE_KIND] = {
   "T_ENTITY",        "T_ARCH",            "T_PORT_DECL",    "T_FCALL",
   "T_LITERAL",       "T_SIGNAL_DECL",     "T_VAR_DECL",     "T_PROCESS",
   "T_REF",           "T_WAIT",            "T_TYPE_DECL",    "T_VAR_ASSIGN",
   "T_PACKAGE",       "T_SIGNAL_ASSIGN",   "T_QUALIFIED",    "T_ENUM_LIT",
   "T_CONST_DECL",    "T_FUNC_DECL",       "T_ELAB",         "T_AGGREGATE",
   "T_ASSERT",        "T_ATTR_REF",        "T_ARRAY_REF",    "T_ARRAY_SLICE",
   "T_INSTANCE",      "T_IF",              "T_NULL",         "T_PACK_BODY",
   "T_FUNC_BODY",     "T_RETURN",          "T_CASSIGN",      "T_WHILE",
   "T_WAVEFORM",      "T_ALIAS",           "T_FOR",          "T_ATTR_DECL",
   "T_ATTR_SPEC",     "T_PROC_DECL",       "T_PROC_BODY",    "T_EXIT",
   "T_PCALL",         "T_CASE",            "T_BLOCK",        "T_COND",
   "T_TYPE_CONV",     "T_SELECT",          "T_COMPONENT",    "T_IF_GENERATE",
   "T_FOR_GENERATE",  "T_FILE_DECL",       "T_OPEN",         "T_FIELD_DECL",
   "T_RECORD_REF",    "T_ALL",             "T_NEW",          "T_CASSERT",
   "T_CPCALL",        "T_UNIT_DECL",       "T_NEXT",         "T_GENVAR",
   "T_PARAM",         "T_ASSOC",           "T_USE",          "T_HIER",
   "T_SPEC",          "T_BINDING",         "T_LIBRARY",      "T_DESIGN_UNIT",
   "T_CONFIGURATION", "T_PROT_BODY",       "T_CONTEXT",      "T_CTXREF",
   "T_CONSTRAINT",    "T_BLOCK_CONFIG",    "T_PROT_FCALL",   "T_PROT_PCALL",
   "T_RANGE",         "T_IMPLICIT_SIGNAL", "T_DISCONNECT",   "T_GROUP_TEMPLATE",
   "T_GROUP",         "T_SUBTYPE_DECL"
};

static const change_allowed_t change_allowed[] = {
   { T_REF,         T_FCALL         },
   { T_REF,         T_PCALL         },
   { T_ARRAY_REF,   T_FCALL         },
   { T_FCALL,       T_ARRAY_REF     },
   { T_FCALL,       T_PCALL         },
   { T_FCALL,       T_TYPE_CONV     },
   { T_REF,         T_TYPE_CONV     },
   { T_REF,         T_RECORD_REF    },
   { T_REF,         T_QUALIFIED     },
   { T_ARRAY_REF,   T_ARRAY_SLICE   },
   { T_ASSERT,      T_CASSERT       },
   { T_DESIGN_UNIT, T_ENTITY        },
   { T_DESIGN_UNIT, T_PACKAGE       },
   { T_DESIGN_UNIT, T_PACK_BODY     },
   { T_DESIGN_UNIT, T_ARCH          },
   { T_DESIGN_UNIT, T_CONFIGURATION },
   { T_DESIGN_UNIT, T_CONTEXT       },
   { T_FUNC_DECL,   T_FUNC_BODY     },
   { T_PROC_DECL,   T_PROC_BODY     },
   { T_REF,         T_ARRAY_SLICE   },
   { T_FCALL,       T_CPCALL        },
   { T_PCALL,       T_CPCALL        },
   { T_REF,         T_CPCALL        },
   { T_ATTR_REF,    T_ARRAY_REF     },
   { T_PROT_FCALL,  T_PROT_PCALL    },
   { T_FCALL,       T_PROT_FCALL    },
   { T_PCALL,       T_PROT_PCALL    },
   { -1,            -1              }
};

struct _tree {
   object_t object;
};

struct _type {
   object_t object;
};

struct _e_node {
   object_t object;
};

static const tree_kind_t stmt_kinds[] = {
   T_PROCESS, T_WAIT,        T_VAR_ASSIGN,   T_SIGNAL_ASSIGN,
   T_ASSERT,  T_INSTANCE,    T_IF,           T_NULL,
   T_RETURN,  T_CASSIGN,     T_WHILE,        T_FOR,
   T_EXIT,    T_PCALL,       T_CASE,         T_BLOCK,
   T_SELECT,  T_IF_GENERATE, T_FOR_GENERATE, T_CPCALL,
   T_CASSERT, T_NEXT,        T_PROT_PCALL
};

static tree_kind_t expr_kinds[] = {
   T_FCALL,     T_LITERAL,   T_REF,        T_QUALIFIED,
   T_AGGREGATE, T_ATTR_REF,  T_ARRAY_REF,  T_ARRAY_SLICE,
   T_TYPE_CONV, T_OPEN,      T_RECORD_REF, T_ALL,
   T_NEW,       T_PROT_FCALL
};

static tree_kind_t decl_kinds[] = {
   T_PORT_DECL,  T_SIGNAL_DECL,    T_VAR_DECL,     T_TYPE_DECL,
   T_CONST_DECL, T_FUNC_DECL,      T_FUNC_BODY,    T_ALIAS,
   T_ATTR_DECL,  T_ATTR_SPEC,      T_PROC_DECL,    T_PROC_BODY,
   T_COMPONENT,  T_FILE_DECL,      T_FIELD_DECL,   T_UNIT_DECL,
   T_GENVAR,     T_HIER,           T_SPEC,         T_BINDING,
   T_USE,        T_PROT_BODY,      T_BLOCK_CONFIG, T_IMPLICIT_SIGNAL,
   T_DISCONNECT, T_GROUP_TEMPLATE, T_GROUP,        T_SUBTYPE_DECL
};

object_class_t tree_object = {
   .name           = "tree",
   .change_allowed = change_allowed,
   .has_map        = has_map,
   .kind_text_map  = kind_text_map,
   .tag            = OBJECT_TAG_TREE,
   .last_kind      = T_LAST_TREE_KIND,
   .gc_roots       = { T_ARCH, T_ENTITY, T_PACKAGE, T_ELAB, T_PACK_BODY,
                       T_CONTEXT, T_CONFIGURATION, T_DESIGN_UNIT },
   .gc_num_roots   = 8
};

object_arena_t *global_arena = NULL;

static bool tree_kind_in(tree_t t, const tree_kind_t *list, size_t len)
{
   for (size_t i = 0; i < len; i++) {
      if (t->object.kind == list[i])
         return true;
   }

   return false;
}

static void tree_assert_kind(tree_t t, const tree_kind_t *list, size_t len,
                             const char *what)
{
   LCOV_EXCL_START
   if (unlikely(!tree_kind_in(t, list, len)))
      fatal_trace("tree kind %s is not %s",
                  tree_kind_str(t->object.kind), what);
   LCOV_EXCL_STOP
}

static void tree_assert_stmt(tree_t t)
{
   tree_assert_kind(t, stmt_kinds, ARRAY_LEN(stmt_kinds), "a statement");
}

static void tree_assert_expr(tree_t t)
{
   tree_assert_kind(t, expr_kinds, ARRAY_LEN(expr_kinds), "an expression");
}

static void tree_assert_decl(tree_t t)
{
   tree_assert_kind(t, decl_kinds, ARRAY_LEN(decl_kinds), "a declaration");
}

static inline tree_t tree_array_nth(item_t *item, unsigned n)
{
   return container_of(AGET(item->obj_array, n), struct _tree, object);
}

static inline void tree_array_add(item_t *item, tree_t t)
{
   APUSH(item->obj_array, &(t->object));
}

static inline void tree_array_insert(item_t *item, unsigned opos, tree_t new)
{
   assert(opos <= item->obj_array.count);

   if (opos == item->obj_array.count)
      APUSH(item->obj_array, &(new->object));
   else {
      ARESIZE(item->obj_array, item->obj_array.count + 1);
      memmove(item->obj_array.items + opos + 1,
              item->obj_array.items + opos,
              (item->obj_array.count - 1 - opos) * sizeof(object_t*));
      item->obj_array.items[opos] = &(new->object);
   }
}

tree_t tree_new(tree_kind_t kind)
{
   return (tree_t)object_new(global_arena, &tree_object, kind);
}

const loc_t *tree_loc(tree_t t)
{
   assert(t != NULL);

   return &t->object.loc;
}

void tree_set_loc(tree_t t, const loc_t *loc)
{
   assert(t != NULL);
   assert(loc != NULL);

   t->object.loc = *loc;
}

ident_t tree_ident(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_IDENT);
   assert(item->ident != NULL);
   return item->ident;
}

bool tree_has_ident(tree_t t)
{
   return lookup_item(&tree_object, t, I_IDENT)->ident != NULL;
}

void tree_set_ident(tree_t t, ident_t i)
{
   lookup_item(&tree_object, t, I_IDENT)->ident = i;
}

ident_t tree_ident2(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_IDENT2);
   assert(item->ident != NULL);
   return item->ident;
}

void tree_set_ident2(tree_t t, ident_t i)
{
   lookup_item(&tree_object, t, I_IDENT2)->ident = i;
}

bool tree_has_ident2(tree_t t)
{
   return lookup_item(&tree_object, t, I_IDENT2)->ident != NULL;
}

tree_kind_t tree_kind(tree_t t)
{
   assert(t != NULL);
   return t->object.kind;
}

void tree_change_kind(tree_t t, tree_kind_t kind)
{
   object_change_kind(&tree_object, &(t->object), kind);
}

unsigned tree_ports(tree_t t)
{
   return lookup_item(&tree_object, t, I_PORTS)->obj_array.count;
}

tree_t tree_port(tree_t t, unsigned n)
{
   item_t *item = lookup_item(&tree_object, t, I_PORTS);
   return tree_array_nth(item, n);
}

void tree_add_port(tree_t t, tree_t d)
{
   tree_assert_decl(d);
   tree_array_add(lookup_item(&tree_object, t, I_PORTS), d);
   object_write_barrier(&(t->object), &(d->object));
}

unsigned tree_subkind(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_SUBKIND);
   return item->ival;
}

void tree_set_subkind(tree_t t, unsigned sub)
{
   lookup_item(&tree_object, t, I_SUBKIND)->ival = sub;
}

unsigned tree_generics(tree_t t)
{
   return lookup_item(&tree_object, t, I_GENERICS)->obj_array.count;
}

tree_t tree_generic(tree_t t, unsigned n)
{
   item_t *item = lookup_item(&tree_object, t, I_GENERICS);
   return tree_array_nth(item, n);
}

void tree_add_generic(tree_t t, tree_t d)
{
   tree_assert_decl(d);
   tree_array_add(lookup_item(&tree_object, t, I_GENERICS), d);
   object_write_barrier(&(t->object), &(d->object));
}

type_t tree_type(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_TYPE);
   assert(item->type != NULL);
   return item->type;
}

void tree_set_type(tree_t t, type_t ty)
{
   lookup_item(&tree_object, t, I_TYPE)->type = ty;
   object_write_barrier(&(t->object), &(ty->object));
}

bool tree_has_type(tree_t t)
{
   return lookup_item(&tree_object, t, I_TYPE)->type != NULL;
}

unsigned tree_params(tree_t t)
{
   return lookup_item(&tree_object, t, I_PARAMS)->obj_array.count;
}

tree_t tree_param(tree_t t, unsigned n)
{
   item_t *item = lookup_item(&tree_object, t, I_PARAMS);
   return tree_array_nth(item, n);
}

void tree_add_param(tree_t t, tree_t e)
{
   assert(tree_kind(e) == T_PARAM);
   tree_assert_expr(tree_value(e));

   tree_array_add(lookup_item(&tree_object, t, I_PARAMS), e);
   object_write_barrier(&(t->object), &(e->object));
}

unsigned tree_genmaps(tree_t t)
{
   return lookup_item(&tree_object, t, I_GENMAPS)->obj_array.count;
}

tree_t tree_genmap(tree_t t, unsigned n)
{
   item_t *item = lookup_item(&tree_object, t, I_GENMAPS);
   return tree_array_nth(item, n);
}

void tree_add_genmap(tree_t t, tree_t e)
{
   tree_assert_expr(tree_value(e));
   tree_array_add(lookup_item(&tree_object, t, I_GENMAPS), e);
}

int64_t tree_ival(tree_t t)
{
   return lookup_item(&tree_object, t, I_IVAL)->ival;
}

void tree_set_ival(tree_t t, int64_t i)
{
   lookup_item(&tree_object, t, I_IVAL)->ival = i;
}

double tree_dval(tree_t t)
{
   return lookup_item(&tree_object, t, I_DVAL)->dval;
}

void tree_set_dval(tree_t t, double d)
{
   lookup_item(&tree_object, t, I_DVAL)->dval = d;
}

tree_flags_t tree_flags(tree_t t)
{
   return lookup_item(&tree_object, t, I_FLAGS)->ival;
}

void tree_set_flag(tree_t t, tree_flags_t mask)
{
   lookup_item(&tree_object, t, I_FLAGS)->ival |= mask;
}

void tree_clear_flag(tree_t t, tree_flags_t mask)
{
   lookup_item(&tree_object, t, I_FLAGS)->ival &= ~mask;
}

e_node_t tree_eopt(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_EOPT);
   assert(item->object != NULL);
   return container_of(item->object, struct _e_node, object);
}

void tree_set_eopt(tree_t t, e_node_t e)
{
   lookup_item(&tree_object, t, I_EOPT)->object = &(e->object);
   object_write_barrier(&(t->object), &(e->object));
}

tree_t tree_primary(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_PRIMARY);
   assert(item->object != NULL);
   return container_of(item->object, struct _tree, object);
}

bool tree_has_primary(tree_t t)
{
   return lookup_item(&tree_object, t, I_PRIMARY)->object != NULL;
}

void tree_set_primary(tree_t t, tree_t unit)
{
   lookup_item(&tree_object, t, I_PRIMARY)->object = &(unit->object);
   object_write_barrier(&(t->object), &(unit->object));
}

unsigned tree_chars(tree_t t)
{
   assert((t->object.kind == T_LITERAL) && (tree_subkind(t) == L_STRING));
   return lookup_item(&tree_object, t, I_CHARS)->obj_array.count;
}

tree_t tree_char(tree_t t, unsigned n)
{
   assert((t->object.kind == T_LITERAL) && (tree_subkind(t) == L_STRING));
   item_t *item = lookup_item(&tree_object, t, I_CHARS);
   return tree_array_nth(item, n);
}

void tree_add_char(tree_t t, tree_t ref)
{
   assert((t->object.kind == T_LITERAL) && (tree_subkind(t) == L_STRING));
   tree_array_add(lookup_item(&tree_object, t, I_CHARS), ref);
   object_write_barrier(&(t->object), &(ref->object));
}

bool tree_has_value(tree_t t)
{
   return lookup_item(&tree_object, t, I_VALUE)->object != NULL;
}

tree_t tree_value(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_VALUE);
   assert(item->object != NULL);
   return container_of(item->object, struct _tree, object);
}

void tree_set_value(tree_t t, tree_t v)
{
   if ((v != NULL) && (t->object.kind != T_ASSOC) && (t->object.kind != T_SPEC))
      tree_assert_expr(v);
   lookup_item(&tree_object, t, I_VALUE)->object = &(v->object);
   object_write_barrier(&(t->object), &(v->object));
}

unsigned tree_decls(tree_t t)
{
   return lookup_item(&tree_object, t, I_DECLS)->obj_array.count;
}

tree_t tree_decl(tree_t t, unsigned n)
{
   item_t *item = lookup_item(&tree_object, t, I_DECLS);
   return tree_array_nth(item, n);
}

void tree_add_decl(tree_t t, tree_t d)
{
   tree_assert_decl(d);
   tree_array_add(lookup_item(&tree_object, t, I_DECLS), d);
   object_write_barrier(&(t->object), &(d->object));
}

void tree_insert_decl(tree_t t, unsigned pos, tree_t d)
{
   tree_assert_decl(d);
   tree_array_insert(lookup_item(&tree_object, t, I_DECLS), pos, d);
   object_write_barrier(&(t->object), &(d->object));
}

unsigned tree_stmts(tree_t t)
{
   return lookup_item(&tree_object, t, I_STMTS)->obj_array.count;
}

tree_t tree_stmt(tree_t t, unsigned n)
{
   item_t *item = lookup_item(&tree_object, t, I_STMTS);
   return tree_array_nth(item, n);
}

void tree_add_stmt(tree_t t, tree_t s)
{
   tree_assert_stmt(s);
   tree_array_add(lookup_item(&tree_object, t, I_STMTS), s);
   object_write_barrier(&(t->object), &(s->object));
}

unsigned tree_waveforms(tree_t t)
{
   return lookup_item(&tree_object, t, I_WAVES)->obj_array.count;
}

tree_t tree_waveform(tree_t t, unsigned n)
{
   item_t *item = lookup_item(&tree_object, t, I_WAVES);
   return tree_array_nth(item, n);
}

void tree_add_waveform(tree_t t, tree_t w)
{
   assert(w->object.kind == T_WAVEFORM);
   tree_array_add(lookup_item(&tree_object, t, I_WAVES), w);
   object_write_barrier(&(t->object), &(w->object));
}

unsigned tree_else_stmts(tree_t t)
{
   return lookup_item(&tree_object, t, I_ELSES)->obj_array.count;
}

tree_t tree_else_stmt(tree_t t, unsigned n)
{
   item_t *item = lookup_item(&tree_object, t, I_ELSES);
   return tree_array_nth(item, n);
}

void tree_add_else_stmt(tree_t t, tree_t s)
{
   tree_assert_stmt(s);
   tree_array_add(lookup_item(&tree_object, t, I_ELSES), s);
   object_write_barrier(&(t->object), &(s->object));
}

unsigned tree_conds(tree_t t)
{
   return lookup_item(&tree_object, t, I_CONDS)->obj_array.count;
}

tree_t tree_cond(tree_t t, unsigned n)
{
   item_t *item = lookup_item(&tree_object, t, I_CONDS);
   return tree_array_nth(item, n);
}

void tree_add_cond(tree_t t, tree_t c)
{
   assert(c->object.kind == T_COND);
   tree_array_add(lookup_item(&tree_object, t, I_CONDS), c);
   object_write_barrier(&(t->object), &(c->object));
}

bool tree_has_delay(tree_t t)
{
   return lookup_item(&tree_object, t, I_DELAY)->object != NULL;
}

tree_t tree_delay(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_DELAY);
   assert(item->object != NULL);
   return container_of(item->object, struct _tree, object);
}

void tree_set_delay(tree_t t, tree_t d)
{
   tree_assert_expr(d);
   lookup_item(&tree_object, t, I_DELAY)->object = &(d->object);
   object_write_barrier(&(t->object), &(d->object));
}

unsigned tree_triggers(tree_t t)
{
   return lookup_item(&tree_object, t, I_TRIGGERS)->obj_array.count;
}

tree_t tree_trigger(tree_t t, unsigned n)
{
   item_t *item = lookup_item(&tree_object, t, I_TRIGGERS);
   return tree_array_nth(item, n);
}

void tree_add_trigger(tree_t t, tree_t s)
{
   tree_assert_expr(s);
   tree_array_add(lookup_item(&tree_object, t, I_TRIGGERS), s);
   object_write_barrier(&(t->object), &(s->object));
}

tree_t tree_target(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_TARGET);
   assert(item->object != NULL);
   return container_of(item->object, struct _tree, object);
}

void tree_set_target(tree_t t, tree_t lhs)
{
   lookup_item(&tree_object, t, I_TARGET)->object = &(lhs->object);
   object_write_barrier(&(t->object), &(lhs->object));
}

tree_t tree_ref(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_REF);
   assert(item->object != NULL);
   return container_of(item->object, struct _tree, object);
}

bool tree_has_ref(tree_t t)
{
   return lookup_item(&tree_object, t, I_REF)->object != NULL;
}

void tree_set_ref(tree_t t, tree_t decl)
{
   lookup_item(&tree_object, t, I_REF)->object = &(decl->object);
   object_write_barrier(&(t->object), &(decl->object));
}

tree_t tree_spec(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_SPEC);
   assert(item->object != NULL);
   return container_of(item->object, struct _tree, object);
}

bool tree_has_spec(tree_t t)
{
   return lookup_item(&tree_object, t, I_SPEC)->object != NULL;
}

void tree_set_spec(tree_t t, tree_t s)
{
   lookup_item(&tree_object, t, I_SPEC)->object = &(s->object);
   object_write_barrier(&(t->object), &(s->object));
}

unsigned tree_contexts(tree_t t)
{
   return lookup_item(&tree_object, t, I_CONTEXT)->obj_array.count;
}

tree_t tree_context(tree_t t, unsigned n)
{
   item_t *item = lookup_item(&tree_object, t, I_CONTEXT);
   return tree_array_nth(item, n);
}

void tree_add_context(tree_t t, tree_t ctx)
{
   assert(ctx->object.kind == T_USE || ctx->object.kind == T_LIBRARY
          || ctx->object.kind == T_CTXREF);
   tree_array_add(lookup_item(&tree_object, t, I_CONTEXT), ctx);
   object_write_barrier(&(t->object), &(ctx->object));
}

unsigned tree_assocs(tree_t t)
{
   return lookup_item(&tree_object, t, I_ASSOCS)->obj_array.count;
}

tree_t tree_assoc(tree_t t, unsigned n)
{
   item_t *item = lookup_item(&tree_object, t, I_ASSOCS);
   return tree_array_nth(item, n);
}

void tree_add_assoc(tree_t t, tree_t a)
{
   assert(a->object.kind == T_ASSOC);
   tree_array_add(lookup_item(&tree_object, t, I_ASSOCS), a);
   object_write_barrier(&(t->object), &(a->object));
}

tree_t tree_severity(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_SEVERITY);
   assert(item->object != NULL);
   return container_of(item->object, struct _tree, object);
}

void tree_set_severity(tree_t t, tree_t s)
{
   tree_assert_expr(s);
   lookup_item(&tree_object, t, I_SEVERITY)->object = &(s->object);
   object_write_barrier(&(t->object), &(s->object));
}

tree_t tree_message(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_MESSAGE);
   assert(item->object != NULL);
   return container_of(item->object, struct _tree, object);
}

bool tree_has_message(tree_t t)
{
   return lookup_item(&tree_object, t, I_MESSAGE)->object != NULL;
}

void tree_set_message(tree_t t, tree_t m)
{
   tree_assert_expr(m);
   lookup_item(&tree_object, t, I_MESSAGE)->object = &(m->object);
   object_write_barrier(&(t->object), &(m->object));
}

void tree_add_range(tree_t t, tree_t r)
{
   tree_array_add(lookup_item(&tree_object, t, I_RANGES), r);
   object_write_barrier(&(t->object), &(r->object));
}

tree_t tree_range(tree_t t, unsigned n)
{
   item_t *item = lookup_item(&tree_object, t, I_RANGES);
   return tree_array_nth(item, n);
}

unsigned tree_ranges(tree_t t)
{
   return lookup_item(&tree_object, t, I_RANGES)->obj_array.count;
}

unsigned tree_pos(tree_t t)
{
   return lookup_item(&tree_object, t, I_POS)->ival;
}

void tree_set_pos(tree_t t, unsigned pos)
{
   lookup_item(&tree_object, t, I_POS)->ival = pos;
}

tree_t tree_left(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_LEFT);
   assert(item->object != NULL);
   return container_of(item->object, struct _tree, object);
}

void tree_set_left(tree_t t, tree_t left)
{
   tree_assert_expr(left);
   lookup_item(&tree_object, t, I_LEFT)->object = &(left->object);
   object_write_barrier(&(t->object), &(left->object));
}

tree_t tree_right(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_RIGHT);
   assert(item->object != NULL);
   return container_of(item->object, struct _tree, object);
}

void tree_set_right(tree_t t, tree_t right)
{
   tree_assert_expr(right);
   lookup_item(&tree_object, t, I_RIGHT)->object = &(right->object);
   object_write_barrier(&(t->object), &(right->object));
}

class_t tree_class(tree_t t)
{
   return lookup_item(&tree_object, t, I_CLASS)->ival;
}

void tree_set_class(tree_t t, class_t c)
{
   lookup_item(&tree_object, t, I_CLASS)->ival = c;
}

tree_t tree_reject(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_REJECT);
   assert(item->object != NULL);
   return container_of(item->object, struct _tree, object);
}

void tree_set_reject(tree_t t, tree_t r)
{
   tree_assert_expr(r);
   lookup_item(&tree_object, t, I_REJECT)->object = &(r->object);
   object_write_barrier(&(t->object), &(r->object));
}

bool tree_has_reject(tree_t t)
{
   return lookup_item(&tree_object, t, I_REJECT)->object != NULL;
}

tree_t tree_guard(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_GUARD);
   assert(item->object != NULL);
   return container_of(item->object, struct _tree, object);
}

void tree_set_guard(tree_t t, tree_t g)
{
   assert(g->object.kind == T_REF);
   lookup_item(&tree_object, t, I_GUARD)->object = &(g->object);
   object_write_barrier(&(t->object), &(g->object));
}

bool tree_has_guard(tree_t t)
{
   return lookup_item(&tree_object, t, I_GUARD)->object != NULL;
}

tree_t tree_name(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_NAME);
   assert(item->object != NULL);
   return container_of(item->object, struct _tree, object);
}

void tree_set_name(tree_t t, tree_t n)
{
   tree_assert_expr(n);
   lookup_item(&tree_object, t, I_NAME)->object = &(n->object);
   object_write_barrier(&(t->object), &(n->object));
}

bool tree_has_name(tree_t t)
{
   return lookup_item(&tree_object, t, I_NAME)->object != NULL;
}

tree_t tree_file_mode(tree_t t)
{
   item_t *item = lookup_item(&tree_object, t, I_FILE_MODE);
   return container_of(item->object, struct _tree, object);
}

void tree_set_file_mode(tree_t t, tree_t m)
{
   lookup_item(&tree_object, t, I_FILE_MODE)->object = &(m->object);
   object_write_barrier(&(t->object), &(m->object));
}

unsigned tree_visit(tree_t t, tree_visit_fn_t fn, void *context)
{
   assert(t != NULL);

   object_visit_ctx_t ctx = {
      .count      = 0,
      .postorder  = fn,
      .preorder   = NULL,
      .context    = context,
      .kind       = T_LAST_TREE_KIND,
      .generation = object_next_generation(),
      .deep       = false,
   };

   object_visit(&(t->object), &ctx);

   return ctx.count;
}

unsigned tree_visit_only(tree_t t, tree_visit_fn_t fn,
                         void *context, tree_kind_t kind)
{
   assert(t != NULL);

   object_visit_ctx_t ctx = {
      .count      = 0,
      .postorder  = fn,
      .preorder   = NULL,
      .context    = context,
      .kind       = kind,
      .generation = object_next_generation(),
      .deep       = false
   };

   object_visit(&(t->object), &ctx);

   return ctx.count;
}

void tree_write(tree_t t, fbuf_t *f)
{
   if (global_arena != NULL) {
      object_arena_freeze(global_arena);
      global_arena = NULL;
   }

   object_write(&(t->object), f);
}

tree_t tree_read(fbuf_t *f, tree_load_fn_t find_deps_fn)
{
   object_t *o = object_read(f, (object_load_fn_t)find_deps_fn);
   assert(o->tag == OBJECT_TAG_TREE);
   return container_of(o, struct _tree, object);
}

tree_t tree_rewrite(tree_t t, tree_rewrite_pre_fn_t pre_fn,
                    tree_rewrite_post_fn_t post_fn, void *context)
{
   object_rewrite_ctx_t ctx = {
      .generation = object_next_generation(),
      .pre_fn     = (object_rewrite_pre_fn_t)pre_fn,
      .post_fn    = (object_rewrite_post_fn_t)post_fn,
      .context    = context,
      .arena      = global_arena,
      .tag        = OBJECT_TAG_TREE
   };

   object_t *result = object_rewrite(&(t->object), &ctx);
   free(ctx.cache);
   return container_of(result, struct _tree, object);
}

tree_t tree_copy(tree_t t, tree_copy_pred_t pred,
                 tree_copy_fn_t tree_callback,
                 type_copy_fn_t type_callback,
                 void *context)
{
   object_copy_ctx_t ctx = {
      .generation  = object_next_generation(),
      .should_copy = (object_copy_pred_t)pred,
      .context     = context,
      .arena       = global_arena,
      .tag         = OBJECT_TAG_TREE
   };

   ctx.callback[OBJECT_TAG_TREE] = (object_copy_fn_t)tree_callback;
   ctx.callback[OBJECT_TAG_TYPE] = (object_copy_fn_t)type_callback;

   return (tree_t)object_copy(&(t->object), &ctx);
}

const char *tree_kind_str(tree_kind_t t)
{
   return kind_text_map[t];
}

void make_new_arena(void)
{
   if (global_arena != NULL)
      object_arena_freeze(global_arena);

   global_arena = object_arena_new(OBJECT_ARENA_SZ, standard());
}

object_arena_t *tree_arena(tree_t t)
{
   return object_arena(&(t->object));
}

void tree_walk_deps(tree_t t, tree_deps_fn_t fn, void *ctx)
{
   object_arena_walk_deps(object_arena(&(t->object)), fn, ctx);
}
