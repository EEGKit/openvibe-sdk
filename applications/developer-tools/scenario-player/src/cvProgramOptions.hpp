#pragma once

#include <iostream>
#include <map>
#include <regex>

#include <boost/variant.hpp>

#include "cvsp_defines.h"

namespace CertiViBE
{
	/**
	* \struct ProgramOptionsTraits
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Helper class for ProgramOptions type checking
	* \ingroup ScenarioPlayer
	*
	* This class provides aliases to types currently handled by ProgramOptions class
	* as well as type checking meta-programming features.
	*
	*/
	struct ProgramOptionsTraits
	{
		// types handled by ProgramOptions
		using TokenPair = std::pair<std::string, std::string>;
		using String = std::string;
		using Integer = int;
		using Float = double;

		using StringList = std::vector<std::string>;
		using IntegerList = std::vector<int>;
		using FloatList = std::vector<double>;
		using TokenPairList = std::vector<TokenPair>;
	};

	namespace ProgramOptionsUtils
	{
		// template meta-programming features

		/* Base type that represents a true value at compile-time */
		struct TrueType {
			static const bool value = true;
		};

		/* Base type that represents a false value at compile-time */
		struct FalseType {
			static const bool value = false;
		};

		/* IsCompliant is used to statically check if a type is compliant with the handled types */
		template <typename T>
		struct IsCompliant : FalseType
		{
		};

		/* Specialization of the template for compliant types */
		template <>
		struct IsCompliant<ProgramOptionsTraits::TokenPair> : TrueType
		{};

		/* Specialization of the template for compliant types */
		template <>
		struct IsCompliant<ProgramOptionsTraits::TokenPairList> : TrueType
		{};

		/* Specialization of the template for compliant types */
		template <>
		struct IsCompliant<ProgramOptionsTraits::String> : TrueType
		{};

		/* Specialization of the template for compliant types */
		template <>
		struct IsCompliant<ProgramOptionsTraits::StringList> : TrueType
		{};

		/* Specialization of the template for compliant types */
		template <>
		struct IsCompliant<ProgramOptionsTraits::Integer> : TrueType
		{};

		/* Specialization of the template for compliant types */
		template <>
		struct IsCompliant<ProgramOptionsTraits::IntegerList> : TrueType
		{};

		/* Specialization of the template for compliant types */
		template <>
		struct IsCompliant<ProgramOptionsTraits::Float> : TrueType
		{};

		/* Specialization of the template for compliant types */
		template <>
		struct IsCompliant<ProgramOptionsTraits::FloatList> : TrueType
		{};

		/* IsSignatureCompliant is used to statically checked a list of type is compliant with handled type */
		template<typename... List>
		struct IsSignatureCompliant;

		/* Specialization for empty list */
		template<>
		struct IsSignatureCompliant<> : TrueType{};

		/* Specialization for non-empty list */
		template <typename Head, typename... Tail>
		struct IsSignatureCompliant<Head, Tail...> :
			std::conditional<IsCompliant<Head>::value, IsSignatureCompliant<Tail...>, FalseType>::type
		{
		};

		/* IsIn is used to statically check if a type T is in a list of types List*/
		template <typename T, typename... List>
		struct IsIn; // interface

		/* Specialization for empty list */
		template<typename T>
		struct IsIn<T> : FalseType{};

		/* Specialization for list where 1st element is a match */
		template <typename T, typename... Tail>
		struct IsIn<T, T, Tail...> : TrueType
		{};

		/* Specialization for list of many elements */
		template <typename T, typename Head, typename... Tail>
		struct IsIn<T, Head, Tail...> : IsIn<T, Tail...>{};

		/* HasDuplicate is used to statically check if a list of types has duplicates*/
		template<typename... List>
		struct HasDuplicate;

		/* Specialization for empty list */
		template<>
		struct HasDuplicate<> : FalseType{};

		/* Specialization for 1-element list */
		template <typename T>
		struct HasDuplicate<T> : FalseType{};

		/* Specialization for list of many elements */
		template <typename Head, typename... Tail>
		struct HasDuplicate<Head, Tail...> :
			std::conditional<IsIn<Head, Tail...>::value, TrueType, HasDuplicate<Tail...>>::type
		{
		};
	}

	/**
	* \class ProgramOptions
	* \author cgarraud (INRIA)
	* \date 2016-01-25
	* \brief Command-line options parser
	* \ingroup ScenarioPlayer
	*
	* This class is a basic class used to parse program options.\n
	* Input format requirements:\n
	* - Option prefix must be '-' or '--' e.g. cmd -help or --help\n
	* - Option value assignment must be set with '=': e.g. cmd -config=myfile.txt\n
	* - Option value that consists of pair are set with (): e.g. cmd -newToken=(key,value)\n
	*
	* Template parameters are the list of option types that can be handled by the class.
	* This list of options must comply to types defined in ProgramOptionsTrait.\n
	*
	* Parsing options occurs in 2 steps:\n
	* - Populating the list of possible options with simple options (e.g. --help) and value options (e.g. --option=value)\n
	* - Parsing options from command line\n
	*
	* 
	* \todo The parser has only be tested for the player use. It needs more in-depth testing
	* to be used in another context. Moreover, it should be extended to accept any type.
	*
	* \note The implementation is trivial. Prefer the use of robust and fully featured
	* boost program_options compiled library if possible.
	*
	*/
	template <typename First, typename... Types>
	class ProgramOptions final
	{

	public:

		// static assert are used to raise understandable at compile time
		static_assert(!ProgramOptionsUtils::HasDuplicate<First, Types...>::value, "Duplicates in the type list");
		static_assert(ProgramOptionsUtils::IsSignatureCompliant<First, Types...>::value, "Types not handled by ProgramOptions");

		/**
		* Struct used to store used-defined option parameters.
		*/
		struct OptionDesc // using a struct allows more extensibility than method parameters
		{
			/** Option shortname (e.g. h for help) */
			std::string m_ShortName;
			/** Option description used for printing option list */
			std::string m_Desc;
		};

		/**
		* \brief Add global description to the list of options
		* \param[in] desc the global description in printable format
		*
		* The global description is used as additional printable documentation
		* when printOptionsDesc() is called.
		*
		*/
		void setGlobalDesc(const std::string& desc);

		/**
		* \brief Add a simple option to the internal dictionary
		* \param[in] name the option name
		* \param[in] optionDesc the option description
		*
		* Simple options are option withou value (e.g. --help or --version)
		*
		*/
		void addSimpleOption(const std::string& name, const OptionDesc& optionDesc);


		/**
		* \brief Add a value option to the internal dictionary
		* \param[in] name the option name
		* \param[in] optionDesc the option description
		* 
		* Template paramter T: The type of the option to be added
		*/
		template <typename T>
		void addValueOption(const std::string& name, const OptionDesc& optionDesc);

		/**
		* \brief Parse command line options
		* \pre addSimpleOption and addValueOption must be called to populate options dictionary
		* \param[in] argc number of arguments
		* \param[in] argv pointer to the list of arguments
		* \return: false if an error occurred during parsing, true otherwise
		*/
		bool parse(int argc, char** argv);

		/**
		* \brief Check if an option was parsed or not
		* \pre Must be called after parse()
		* \param[in] name the option name
		* \return: true if the option was parsed, false otherwise
		*/
		bool hasOption(const std::string& name) const;

		/**
		* \brief Get option value
		* \pre hasOption() should be called to ensure the option is available
		* \param[in] name the option name
		* \return the option value (will be the default value if the option was not parsed)
		*
		* Template paramter T: the type of the option to retrieve (must match the type used to set
		* the option with addValueOption())
		*/
		template <typename T>
		T getOptionValue(const std::string& name) const;


		/**
		* \brief print all option descriptions
		*/
		void printOptionsDesc() const;

	private:

		// The visitor allows us to apply the correct parsing
		// for any type (see boost::variant documentation for more details).
		// Presently, it is pretty ugly. This should be refactored for 
		// as there is a lot of behavorial redundancy.
		class OptionVisitor : public boost::static_visitor<>
		{
		public:

			OptionVisitor(std::string& value) : m_Value(value)
			{
			}

			void operator()(ProgramOptionsTraits::Integer& operand) const;

			void operator()(ProgramOptionsTraits::Float& operand) const;

			void operator()(ProgramOptionsTraits::String& operand) const;

			void operator()(ProgramOptionsTraits::TokenPair& operand) const;

			void operator()(ProgramOptionsTraits::IntegerList& operand) const;

			void operator()(ProgramOptionsTraits::FloatList& operand) const;

			void operator()(ProgramOptionsTraits::StringList& operand) const;

			void operator()(ProgramOptionsTraits::TokenPairList& operand) const;

		private:

			ProgramOptionsTraits::TokenPair parsePair(const std::string& str) const;

			std::string& m_Value;
		};

		using OptionValue = boost::variant<First, Types...>;

		// the pair contais a boolean to quickly know if an option
		// is a simple option or a value option
		using FullOptionDesc = std::pair<bool, OptionDesc>;

		std::string m_GlobalDesc;
		std::map<std::string, FullOptionDesc> m_DescMap;
		std::map<std::string, OptionValue> m_ValueMap;
		std::vector<std::string> m_ParsedOptions;

	};

	///////////////////////////////////////////
	/////// Definition ProgramOptions /////////
	///////////////////////////////////////////

	template <typename First, typename... Types>
	void ProgramOptions<First, Types...>::setGlobalDesc(const std::string& desc)
	{
		m_GlobalDesc = desc;
	}

	template <typename First, typename... Types>
	void ProgramOptions<First, Types...>::addSimpleOption(const std::string& name, const OptionDesc& optionDesc)
	{
		m_DescMap[name] = std::make_pair(true, optionDesc);
	}

	template <typename First, typename... Types>
	template <typename T>
	void ProgramOptions<First, Types...>::addValueOption(const std::string& name, const OptionDesc& optionDesc)
	{
		m_DescMap[name] = std::make_pair(false, optionDesc);

		T defaultValue; // with this implementation, only default constructible type can be added 
		m_ValueMap[name] = defaultValue;
	}

	template <typename First, typename... Types>
	bool ProgramOptions<First, Types...>::hasOption(const std::string& name) const
	{
		return std::find(m_ParsedOptions.begin(), m_ParsedOptions.end(), name) != m_ParsedOptions.end();
	}

	template <typename First, typename... Types>
	template <typename T>
	T ProgramOptions<First, Types...>::getOptionValue(const std::string& name) const
	{
		T value{};

		try
		{
			value = boost::get<T>(m_ValueMap.at(name));
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERROR: Caught exception during option value retrieval: " << e.what() << std::endl;
		}
	
		return value;
	}

	template <typename First, typename... Types>
	bool ProgramOptions<First, Types...>::parse(int argc, char** argv)
	{
		for (int i = 1; i < argc; ++i)
		{
			std::string arg = argv[i];

			auto argSplit = arg.find_first_of("="); // = is the separator for value option

			std::string key;

			if (argSplit == std::string::npos) // simple option
			{
				key = arg;
			}
			else // value option
			{
				key = arg.substr(0, argSplit);
			}

			// first check if the key exists
			auto keyMatch = std::find_if(
				m_DescMap.begin(),
				m_DescMap.end(),
				[&](const std::pair<std::string, FullOptionDesc>& p)
			{
				auto desc = p.second.second;
				return (("-" + p.first) == key) || (("--" + p.first) == key) || (("-" + desc.m_ShortName) == key) || (("--" + desc.m_ShortName) == key);
			}
			);

			if (keyMatch == m_DescMap.end())
			{
				std::cout << "WARNING: Found unknown option: " << key << std::endl;
				std::cout << "Skipping..." << std::endl;
				continue;
			}

			auto desc = keyMatch->second;

			if (!desc.first) // value option
			{
				if (key == arg)
				{
					std::cerr << "ERROR: No value set for argument: " << key << std::endl;
					return false;
				}

				std::string val = arg.substr(argSplit + 1, arg.size() - argSplit - 1); // take value part of the arg

				try
				{
					boost::apply_visitor(OptionVisitor(val), m_ValueMap[keyMatch->first]);
				}
				catch (const std::exception& e)
				{
					std::cerr << "ERROR: Caught exception during option parsing: "<< e.what() << std::endl;
					std::cerr << "Could not parse option with key = " << key << " and value = " << val << std::endl;
					return false;
				}
			}

			m_ParsedOptions.push_back(keyMatch->first);
		}


		return true;
	}

	template <typename First, typename... Types>
	void ProgramOptions<First, Types...>::printOptionsDesc() const
	{
		if (!m_GlobalDesc.empty())
		{
			std::cout << m_GlobalDesc << std::endl;
		}

		std::cout << "List of available options:\n" << std::endl;

		for (auto& option : m_DescMap)
		{
			std::cout << "Option: --" << option.first << std::endl;

			auto desc = option.second.second;

			if (!desc.m_ShortName.empty())
			{
				std::cout << "Shortname: --" << desc.m_ShortName << std::endl;
			}

			std::cout << "Description: " << std::endl;
			std::cout << desc.m_Desc << std::endl << std::endl;
		}
	}

	///////////////////////////////////////////
	/////// Definition Internal Visitor ///////
	///////////////////////////////////////////

	template <typename First, typename... Types>
	void ProgramOptions<First, Types...>::OptionVisitor::operator()(ProgramOptionsTraits::Integer& operand) const
	{
		operand = std::stoi(m_Value);
	}

	template <typename First, typename... Types>
	void ProgramOptions<First, Types...>::OptionVisitor::operator()(ProgramOptionsTraits::Float& operand) const
	{
		operand = std::stod(m_Value);
	}

	template <typename First, typename... Types>
	void ProgramOptions<First, Types...>::OptionVisitor::operator()(ProgramOptionsTraits::String& operand) const
	{
		operand = m_Value;
	}

	template <typename First, typename... Types>
	void ProgramOptions<First, Types...>::OptionVisitor::operator()(ProgramOptionsTraits::TokenPair& operand) const
	{
		operand = this->parsePair(m_Value);
	}

	template <typename First, typename... Types>
	void ProgramOptions<First, Types...>::OptionVisitor::operator()(ProgramOptionsTraits::IntegerList& operand) const
	{
		operand.push_back(std::stoi(m_Value));
	}

	template <typename First, typename... Types>
	void ProgramOptions<First, Types...>::OptionVisitor::operator()(ProgramOptionsTraits::FloatList& operand) const
	{
		operand.push_back(std::stod(m_Value));
	}

	template <typename First, typename... Types>
	void ProgramOptions<First, Types...>::OptionVisitor::operator()(ProgramOptionsTraits::StringList& operand) const
	{
		operand.push_back(m_Value);
	}

	template <typename First, typename... Types>
	void ProgramOptions<First, Types...>::OptionVisitor::operator()(ProgramOptionsTraits::TokenPairList& operand) const
	{
		operand.push_back(this->parsePair(m_Value));
	}

	template <typename First, typename... Types>
	ProgramOptionsTraits::TokenPair ProgramOptions<First, Types...>::OptionVisitor::parsePair(const std::string& str) const
	{
		std::regex pairRegex("\\(.+,.+\\)"); // (a,b) pattern

		if (!std::regex_match(str, pairRegex))
		{
			throw std::runtime_error("Failed to parse pair from value: " + str);
		}

		auto split = m_Value.find_first_of(",");

		return std::make_pair(m_Value.substr(1, split - 1), m_Value.substr(split + 1, m_Value.size() - split - 2));
	}
}