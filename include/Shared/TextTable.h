#include <iostream>
#include <map>
#include <iomanip>
#include <vector>
#include <string>

const std::string blue("\033[0;34m");
const std::string red("\033[0;31m");
const std::string green("\033[1;32m");
const std::string yellow("\033[1;33m");
const std::string cyan("\033[0;36m");
const std::string magenta("\033[0;35m");
const std::string reset("\033[0m");

class TextTable
{
public:
	enum class Alignment { LEFT, RIGHT };
	
	typedef std::vector< std::string > Row;
	
	TextTable(char horizontal = '-', char vertical = '|', char corner = '+');

	void setAlignment(unsigned i, Alignment alignment);

	Alignment alignment(unsigned i) const;

	char vertical() const;

	char horizontal() const;

	void add(std::string const& content, std::string color = reset);

	void endOfRow();

	template <typename Iterator>
	void addRow(Iterator begin, Iterator end)
	{
		for (auto i = begin; i != end; ++i)
		{
			add(*i);
		}

		endOfRow();
	}

	template <typename Container>
	void addRow(Container const& container)
	{
		addRow(container.begin(), container.end());
	}

	std::vector< Row > const& rows() const;

	std::vector< Row > const& rows_color() const;

	void setup() const;

	std::string ruler() const;

	int width(unsigned i) const;

private:
	char _horizontal;
	char _vertical;
	char _corner;
	Row _current;
	Row _current_color;

	std::vector< Row > _rows;
	std::vector< Row > _rows_color;

	std::vector< unsigned > mutable _width;
	std::map< unsigned, Alignment > mutable _alignment;

	static std::string repeat(unsigned times, char c);

	unsigned columns() const;

	void determineWidths() const;

	void setupAlignment() const;
};

std::ostream& operator<<(std::ostream& stream, TextTable const& table);