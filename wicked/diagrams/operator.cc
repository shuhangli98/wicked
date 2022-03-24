#include "helpers/combinatorics.h"
#include "helpers/helpers.h"
#include "helpers/orbital_space.h"

#include "operator.h"

using namespace std;

Operator::Operator(const std::string &label, const std::vector<int> &cre,
                   const std::vector<int> &ann)
    : label_(label), vertex_(cre, ann) {}

Operator::Operator(const std::string &label, const Vertex &vertex)
    : label_(label), vertex_(vertex) {}

const std::string &Operator::label() const { return label_; }

Vertex Operator::vertex() const { return vertex_; }

scalar_t Operator::factor() const {
  scalar_t result = 1;
  for (int s = 0; s < osi->num_spaces(); ++s) {
    result /= static_cast<scalar_t>(factorial(cre(s)));
  }
  for (int s = 0; s < osi->num_spaces(); ++s) {
    result /= static_cast<scalar_t>(factorial(ann(s)));
  }
  return result;
}

Operator Operator::adjoint() const {
  return Operator(label(), vertex().adjoint());
}

int Operator::cre(int space) const { return vertex_.cre(space); }

int Operator::ann(int space) const { return vertex_.ann(space); }

int Operator::num_ops() const { return vertex_.num_ops(); }

bool Operator::operator<(Operator const &other) const {
  // Compare the labels
  if (label_ < other.label_)
    return true;
  if (label_ > other.label_)
    return false;
  // Compare the vertices
  return vertex_ < other.vertex_;
}

std::string Operator::str() const {
  std::vector<std::string> s;
  if (factor() != scalar_t(1)) {
    s.push_back(factor().str(false));
  }
  s.push_back(label_);
  s.push_back("{");
  for (int i = 0; i < osi->num_spaces(); ++i) {
    for (int j = 0; j < cre(i); j++) {
      std::string op_s(1, osi->label(i));
      s.push_back(op_s + "+");
    }
  }

  for (int i = osi->num_spaces() - 1; i >= 0; --i) {
    for (int j = 0; j < ann(i); j++)
      s.push_back(std::string(1, osi->label(i)));
  }

  s.push_back("}");

  return join(s, " ");
}

std::ostream &operator<<(std::ostream &os, const Operator &op) {
  os << op.str();
  return os;
}

bool do_operators_commute(const Operator &a, const Operator &b) {
  int noncommuting = 0;
  for (int s = 0; s < osi->num_spaces(); s++) {
    noncommuting += a.ann(s) * b.cre(s) + a.cre(s) * b.ann(s);
  }
  return noncommuting == 0;
}

bool operator_noncommuting_less(const Operator &a, const Operator &b) {
  // if the operators commute return normal less
  if (do_operators_commute(a, b)) {
    return (a < b);
  }
  return false;
}

Operator make_diag_operator(const std::string &label,
                            const std::vector<char> &cre_labels,
                            const std::vector<char> &ann_labels) {
  // count the number of creation and annihilation operators in each space
  std::vector<int> cre(osi->num_spaces());
  std::vector<int> ann(osi->num_spaces());
  for (const auto &l : cre_labels) {
    int space = osi->label_to_space(l);
    cre[space] += 1;
  }
  for (const auto &l : ann_labels) {
    int space = osi->label_to_space(l);
    ann[space] += 1;
  }

  return Operator(label, cre, ann);
}

int sum_num_ops(const std::vector<Operator> &ops) {
  int r = 0;
  for (const auto &op : ops) {
    r += op.num_ops();
  }
  return r;
}
