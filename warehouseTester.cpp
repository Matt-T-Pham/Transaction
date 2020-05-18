#include "warehouse.h"
#include <iostream>
#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <map>

using namespace std;
using namespace boost::gregorian;

int main()
{
  string slc = "slc";
  warehouse w;

  // std::map <std::string,std::map<boost::gregorian::date, int> > holder;
 
  const std::locale fmt(std::locale::classic(),new boost::gregorian::date_facet("%m/%d/%Y"));

  string date1("06/19/1989");
  string date2("02/13/2013");
  string date3("03/13/4234");

  date d3 = from_us_string(date3);
  date d  = from_us_string(date1);
  date d2 = from_us_string(date2);
  
  ostringstream oss;
  oss.imbue(fmt);
  oss << d;
  
  //  std::string str = oss.str();
  
  // cout << str << endl;

  std::string upc2 = "12312231434251234";
  std::string upc = "1112312312312123123";
  
  int exp = 1;
  int exp2 = 2;
  int exp3 =3;


  w.add_to_holder(upc,date1,exp);
  int answer = w.get_qty(upc,date1);

  cout << answer << endl;



  // holder[upc][d] = exp;
  // holder[upc2][d2] = exp2;
  //  holder[upc][d2]=exp3;
  //  holder[upc2][d3]=exp;
  

  //  cout<< holder[upc2][d3]<<endl;
  //  cout << holder[upc][d2]<<endl;
  // cout << holder[upc][d]<< endl;
  // cout << holder[upc2][d2]<<endl;

}
