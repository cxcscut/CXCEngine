#include "Parser.h"

namespace cxc
{

	Parser::Parser() : error_lines(0),Load(false)
	{
		
	}

	Parser::~Parser()
	{

	}

	bool Parser::isLoaded() const noexcept
	{
		return Load;
	}

	bool Parser::GetTokensAttribSet(const std::string &TokenName,std::vector<std::string> &ret) const noexcept
	{
		ret.clear();

		auto it = m_TokenAttribs.find(TokenName);
		if (it != m_TokenAttribs.end())
		{
			ret = it->second;
			return true;
		}
		else
			return false;
	}

	int Parser::tokenize(const std::string &script) noexcept
	{

		int lines = 0;

		std::fstream script_file(script);

		std::string read_line;

		// for tags matching
		std::stack<std::string> WorkingStack;

		// reading script
		while (std::getline(script_file, read_line))
		{
			auto isAnotation = read_line.find_first_of("@@");

			// check if it's not anotation
			if (isAnotation == std::string::npos || isAnotation != 0) {
				auto first_pos = read_line.find_first_of('<');
				auto last_pos = read_line.find_first_of('>');

				// first match
				if (first_pos != std::string::npos && last_pos != std::string::npos)
				{
					if (read_line[first_pos + 1] == '/') // </token>
					{
						if (WorkingStack.empty())
							return PARSER_ERROR;

						std::string token = read_line.substr(first_pos + 2, last_pos - first_pos - 2);

						// error on closing tags if true

						if (WorkingStack.top() != token)
							return lines;
						else
							// tag matched
							WorkingStack.pop();
					}
					else
					{
						std::string token = read_line.substr(first_pos + 1, last_pos - first_pos - 1);

						std::string last_tag = "</" + token + ">";
						if (read_line.find(last_tag) != std::string::npos) // <token> attribs </token>
						{
							// child node
							auto pos_first = read_line.find_first_of('>');
							auto pos_last = read_line.find_last_of('<');

							auto attrib = read_line.substr(pos_first + 1, pos_last - pos_first - 1);

							// pre processing
							AttribPreProcessing(attrib);

							auto it = m_TokenAttribs.find(token);
							if (it != m_TokenAttribs.end())
								it->second.emplace_back(attrib);
							else
							{
								std::vector<std::string> attribs(1, attrib);
								m_TokenAttribs.insert(std::make_pair(token, attribs));
							}
						}
						else // <token>
						{
							// root node
							if (WorkingStack.empty())
								m_TokenTree.insert(std::make_pair(token,std::make_shared<TokenTree>(token)));
							else
							{
								auto prior_token = WorkingStack.top();

								std::shared_ptr<TokenTree> retNodePtr(nullptr);

								// Check the root node
								for (auto &it : m_TokenTree)
									if (it.second->root == prior_token)
										retNodePtr = it.second;

								// check child node
								for (auto it : m_TokenTree)
									if (it.second->findChildNode(prior_token, retNodePtr))
										break;

								assert(retNodePtr != nullptr);

								retNodePtr->addChildNode(std::make_shared<TokenTree>(token));
							}

							WorkingStack.push(token);
						}
					}
				}
			}

			lines++;
		}

		script_file.close();

		return 0;
	}

	bool Parser::ParseScript(const std::string &script_file) noexcept
	{
		auto ret = tokenize(script_file);
		if (ret == 0)
		{
			Load = true;
			std::cout << "Loading script " << script_file << std::endl << std::endl;
			return true;
		}
		else
		{
			error_lines = ret > 0 ? ret : 0;
			std::cout << "Script parsing error at lines " << error_lines << std::endl << std::endl;
			return false;
		}
			
	}

	int Parser::GetErrorLines() const noexcept
	{
		return error_lines;
	}

	void Parser::AttribPreProcessing(std::string &attrib)
	{
		// remove space in the string
		trim(attrib);

		auto pos = attrib.find_first_of('\"');

		while (pos != std::string::npos)
		{
			attrib.erase(pos,1);
			pos = attrib.find_first_of('\"',pos + 1);
		}
	}

	void Parser::trim(std::string &s) {
		int idx = 0;
		if (!s.empty())
		{
			while ((idx = s.find(" ", idx)) != std::string::npos)
				s.erase(idx, 1);
		}
	}
}