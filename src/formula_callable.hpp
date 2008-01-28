#ifndef FORMULA_CALLABLE_HPP_INCLUDED
#define FORMULA_CALLABLE_HPP_INCLUDED

#include <map>
#include <string>

#include "reference_counted_object.hpp"
#include "variant.hpp"

namespace game_logic
{

enum FORMULA_ACCESS_TYPE { FORMULA_READ_ONLY, FORMULA_WRITE_ONLY, FORMULA_READ_WRITE };
struct formula_input {
	std::string name;
	FORMULA_ACCESS_TYPE access;
	explicit formula_input(const std::string& name, FORMULA_ACCESS_TYPE access=FORMULA_READ_WRITE)
			: name(name), access(access)
	{}
};

//interface for objects that can have formulae run on them
class formula_callable : public reference_counted_object {
public:
	variant query_value(const std::string& key) const {
		if(key == "self") {
			return variant(this);
		}
		return get_value(key);
	}

	void mutate_value(const std::string& key, const variant& value) {
		set_value(key, value);
	}

	std::vector<formula_input> inputs() const {
		std::vector<formula_input> res;
		get_inputs(&res);
		return res;
	}

	bool equals(const formula_callable* other) const {
		return do_compare(other) == 0;
	}

	bool less(const formula_callable* other) const {
		return do_compare(other) < 0;
	}

	virtual void get_inputs(std::vector<formula_input>* inputs) const {}
protected:
	virtual ~formula_callable() {}

	virtual void set_value(const std::string& key, const variant& value);
	virtual int do_compare(const formula_callable* callable) const {
		return (int)this - (int)callable;
	}
private:
	virtual variant get_value(const std::string& key) const = 0;
};

class formula_callable_no_ref_count : public formula_callable {
public:
	formula_callable_no_ref_count() {
		turn_reference_counting_off();
	}
	virtual ~formula_callable_no_ref_count() {}
};

class map_formula_callable : public formula_callable {
public:
	explicit map_formula_callable(const formula_callable* fallback=NULL);
	map_formula_callable& add(const std::string& key, const variant& value);
private:
	variant get_value(const std::string& key) const;
	void get_inputs(std::vector<formula_input>* inputs) const;
	std::map<std::string,variant> values_;
	const formula_callable* fallback_;
};

typedef boost::intrusive_ptr<map_formula_callable> map_formula_callable_ptr;
typedef boost::intrusive_ptr<const map_formula_callable> const_map_formula_callable_ptr;

}

#endif
