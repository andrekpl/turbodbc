#pragma once

#include <turbodbc/buffer_size.h>

#include <cpp_odbc/statement.h>
#include <turbodbc/result_sets/result_set.h>
#include <turbodbc/parameter_sets/field_parameter_set.h>
#include <memory>


namespace turbodbc {


class command {
public:
	command(std::shared_ptr<cpp_odbc::statement const> statement,
	        turbodbc::buffer_size buffer_size,
	        std::size_t parameter_sets_to_buffer,
	        bool prefer_unicode,
	        bool use_double_buffering,
	        bool query_db_for_parameter_types);

	/**
	 * @brief Execute the command and initialize result sets if available
	 *        If your query has parameters, make sure to use get_parameters()
	 *        to transfer the parameters before calling execute().
	 */
	void execute();

	/**
	 * @brief Retrieve the pointer to the result set if one exists
	 * @return Returns empty pointer in case the command has not produced results
	 */
	std::shared_ptr<turbodbc::result_sets::result_set> get_results();

	/**
	 * @brief Get a reference to an object that handles all parameters associated
	 *        with this command
	 */
	bound_parameter_set & get_parameters();

	long get_row_count();

	~command();

private:
	std::shared_ptr<cpp_odbc::statement const> statement_;
	bound_parameter_set params_;
	turbodbc::buffer_size buffer_size_;
	bool prefer_unicode_;
	bool use_double_buffering_;
	std::shared_ptr<result_sets::result_set> results_;
};

}
