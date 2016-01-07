#include <pydbc/make_description.h>

#include <pydbc/descriptions.h>
#include <sqlext.h>

#include <stdexcept>
#include <sstream>

namespace pydbc {

namespace {

	SQLULEN const digits_representable_by_long = 18;

	std::unique_ptr<description const> make_decimal_description(cpp_odbc::column_description const & source)
	{
		if (source.size <= digits_representable_by_long) {
			if (source.decimal_digits == 0) {
				return std::unique_ptr<description>(new integer_description);
			} else {
				return std::unique_ptr<description>(new floating_point_description);
			}
		} else {
			// fall back to strings; add two characters for decimal point and sign!
			return std::unique_ptr<description>(new string_description(source.size + 2));
		}
	}

	using description_ptr = description const *;

	struct description_by_value : public boost::static_visitor<description_ptr> {
		description_ptr operator()(long const &) const
		{
			return new integer_description;
		}

		description_ptr operator()(double const &) const
		{
			return new floating_point_description;
		}

		description_ptr operator()(bool const &) const
		{
			return new boolean_description;
		}

		description_ptr operator()(boost::gregorian::date const &) const
		{
			return new date_description;
		}

		description_ptr operator()(boost::posix_time::ptime const &) const
		{
			return new timestamp_description;
		}

		description_ptr operator()(std::string const & s) const
		{
			return new string_description(s.size());
		}
	};

}

std::unique_ptr<description const> make_description(cpp_odbc::column_description const & source)
{
	switch (source.data_type) {
		case SQL_CHAR:
		case SQL_VARCHAR:
		case SQL_LONGVARCHAR:
		case SQL_WVARCHAR:
		case SQL_WLONGVARCHAR:
		case SQL_WCHAR:
			return std::unique_ptr<description>(new string_description(source.size));
		case SQL_INTEGER:
		case SQL_SMALLINT:
		case SQL_BIGINT:
		case SQL_TINYINT:
			return std::unique_ptr<description>(new integer_description);
		case SQL_REAL:
		case SQL_FLOAT:
		case SQL_DOUBLE:
			return std::unique_ptr<description>(new floating_point_description);
		case SQL_BIT:
			return std::unique_ptr<description>(new boolean_description);
		case SQL_NUMERIC:
		case SQL_DECIMAL:
			return make_decimal_description(source);
//			return std::unique_ptr<description>(new number_description);
		case SQL_TYPE_DATE:
			return std::unique_ptr<description>(new date_description);
		case SQL_TYPE_TIMESTAMP:
			return std::unique_ptr<description>(new timestamp_description);
		default:
			std::ostringstream message;
			message << "Error! Unsupported type identifier '" << source.data_type << "'";
			throw std::runtime_error(message.str());
	}
}


std::unique_ptr<description const> make_description(field const & value)
{
	return std::unique_ptr<description const>(boost::apply_visitor(description_by_value{}, value));
}

}
