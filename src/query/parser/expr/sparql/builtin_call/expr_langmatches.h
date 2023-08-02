#pragma once

#include <memory>

#include "query/parser/expr/expr.h"

namespace SPARQL {
class ExprLangMatches : public Expr {
public:
    std::unique_ptr<Expr> expr1;
    std::unique_ptr<Expr> expr2;

    ExprLangMatches(std::unique_ptr<Expr> expr1, std::unique_ptr<Expr> expr2) :
        expr1 (std::move(expr1)),
        expr2 (std::move(expr2)) { }

    virtual std::unique_ptr<Expr> clone() const override {
        return std::make_unique<ExprLangMatches>(expr1->clone(), expr2->clone());
    }

    void accept_visitor(ExprVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::set<VarId> get_all_vars() const override {
        auto expr1_vars = expr1->get_all_vars();
        auto expr2_vars = expr2->get_all_vars();
        expr1_vars.insert(expr2_vars.begin(), expr2_vars.end());
        return expr1_vars;
    }

    bool has_aggregation() const override {
        return expr1->has_aggregation() or expr2->has_aggregation();
    }

    virtual std::ostream& print_to_ostream(std::ostream& os, int indent = 0) const override {
        return os << std::string(indent, ' ') << "LANGMATCHES(" << *expr1 << ", " << *expr2 << ')';
    }
};
} // namespace SPARQL