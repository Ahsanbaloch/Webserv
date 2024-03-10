#ifndef HEADER_H
# define HEADER_H

class Header
{
private:
	/* data */
public:
	Header(/* args */);
	~Header();

	void	parseRequestLine();
	void	parseHeaderFields();

};

#endif
