// **** Authored by Matthews Pham and Hooper ****
// **** Assignment04 for CS 3505 Spring 2018 ****

//#include <iostream>

#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "parser.h"
#include <set>
#include <queue>

using namespace std;
using namespace boost::gregorian;

// Helper struct for receive/request order processing
struct order
{
  string upc;
  long long qty;
  string location;
};

// Comparator helper class for sorting popular items
class Comparator
{
public:
  bool operator()(pair<string, long long> lhs,
		  pair<string, long long> rhs)
  {
    return lhs.second < rhs.second;
  }
};

// Process receive orders
void receive(order, map<string, map<string, map<date, long long > > > & warehouses, 
	     map<string, food> & food_items,
	     date current_date);

// Process request orders
void request(order, map<string, map<string, map<date, long long > > > & warehouses,
	     map<string, long long> & popularity,
	     map<date, set<string> > & underfilled,
	     date current_date);

// Process all orders
void process_orders(
		    queue<order> & receive_orders,
		    queue<order> & request_orders,
		    map<string, map<string, map<date, long long> > > & warehouses,
		    map<string, food> & food_items,
		    map<string, long long> & popularity,
		    map<date, set<string> > & underfilled,
		    date current_date);

int main(int argc, char* argv[])
{
  string file = argv[1];
  ifstream in(file.c_str());
  ofstream report;
  report.open("our_data3_report.txt");

  

  string line;
  parser p;
  date current_date;

  // maps upc strings -> food objects
  map<string, food> food_items;
  
  // maps warehouse names to inventory sorted by expiration date
  map<string, map<string, map<date, long long> > > warehouses;

  // maps dates to a set of underfilled orders
  map<date, set<string> > underfilled;

  // maps food upcs to request popularity
  map<string, long long> popularity;

  // queue of receive orders to be processed once we've received all of them
  queue<order> receive_orders;

  // queue of request orders to be processed after the receive orders
  queue<order> request_orders;

  // maps food upcs to the number of warehouses that have stock of that food item
  map<string, int> well_stocked;

  // maps food upcs to a set of warehouse loctions in which there is stock
  map<string, set<string> > stock;

  // main loop to parse file data
  while (getline(in, line))
  {
    istringstream iss(line);
   
    string firstword;
    iss >> firstword;

    // create food item objects
    if (firstword == "FoodItem") 
    {
      p.get_food(line, food_items);
    }
    
    // populate warehouses map
    else if (firstword == "Warehouse")
    {
      p.get_warehouse(line, warehouses);
    }

    // assign start state
    else if (firstword == "Start")
    {
      current_date = p.getStart(line);
    }

    // enqueue receive orders for the day
    else if (firstword == "Receive:")
    {      
      string upc;
      long long qty;
      string location;      

      iss >> upc;
      iss >> qty;
      iss >> location;

      order receive;
      receive.upc = boost::algorithm::trim_copy(upc);      
      receive.qty = qty;
      receive.location = boost::algorithm::trim_copy(location);

      receive_orders.push(receive);
    }

    // enqueue request orders for the day
    else if (firstword == "Request:")
    {
      string upc;
      long long request_amt;
      long long to_fill;
      string location;

      iss >> upc;
      iss >> request_amt;
      iss >> location;
      
      order request;
      request.upc = boost::algorithm::trim_copy(upc);
      request.qty = request_amt;
      request.location = boost::algorithm::trim_copy(location);

      request_orders.push(request);      
    }

    // end of day's orders;
    //   process queued orders
    //   then advance date
    //   then expire food from warehouse inventory
    else if (firstword == "Next")
    {
      process_orders(receive_orders,
		     request_orders,
		     warehouses,
		     food_items,
		     popularity,
		     underfilled,
		     current_date);

      // begin new day
      date_duration dd(1);
      current_date = current_date + dd;
     
      // expire and de-stock
      for ( map<string, map<string, map<date, long long > > > ::iterator it = warehouses.begin();
	it != warehouses.end(); ++it)
      {    
	for ( map<string, map<date, long long> > ::iterator itt = (it->second).begin();
	  itt != (it->second).end(); ++itt)
	{
	  for (map<date, long long>::iterator iitt = (itt->second).begin();
	   iitt != (itt->second).end(); ++iitt)
	  {
	    string w = it->first;
	    string upc = itt->first;
	    
	    if (iitt->first == current_date || iitt->second == 0)
	      itt->second.erase(iitt);
	  }
	}
      }
    }

    // end of order data
    //   finish last days' orders
    else if (boost::algorithm::trim_copy(line)=="End")
    {
      process_orders(receive_orders,
		     request_orders,
		     warehouses,
		     food_items,
		     popularity,
		     underfilled,
		     current_date);
    }

    if (in.fail())
	break;
  }  
 
  // generate reports
  cout << "Report by Matthews Pham and Hooper\n";
  cout << endl;

  report << "Report by Matthews Pham and Hooper\n";
  report << endl;

  // generate underfilled orders report
  cout << "Underfilled orders:\n";
  report << "Underfilled orders:\n";
  for (map<date, set<string> >::iterator it = underfilled.begin(); 
       it != underfilled.end(); ++it)
  {
    for (set<string>::iterator itt = it->second.begin();
	 itt != it->second.end(); ++itt)
    {
      cout << p.parse_date(it->first) << " " 
	   << *itt << " " 
	   << food_items[*itt].get_name() << endl;
      report << p.parse_date(it->first) << " " 
	   << *itt << " " 
	   << food_items[*itt].get_name() << endl;
    }
  }
  cout << endl;
  report << endl;

  cout << "Well-Stocked Products:\n";
  report << "Well-Stocked Products:\n";

  // generate well-stocked report
  for ( map<string, map<string, map<date, long long > > > ::iterator it = warehouses.begin();
  	it != warehouses.end(); ++it)
  {
    for ( map<string, map<date, long long> > ::iterator itt = (it->second).begin();
  	  itt != (it->second).end(); ++itt)
    {
      for ( map<date, long long>::iterator iitt = (itt->second).begin();
	    iitt != (itt->second).end(); ++iitt)
      {
	if (iitt->second > 0)
	{
	  stock[itt->first].insert(it->first);
	}

	//if(well_stocked.count(itt->first) == 1 && iitt->second >= 1)
	//{
	//  well_stocked[itt->first]++;	 
	//}
	//else
	//{
        //  well_stocked[itt->first] = 0;
	//}
      }
    }
  }

  for (map<string, set<string> >::iterator it = stock.begin();
       it != stock.end(); it++)
  {
    if (it->second.size() > 1)
    {
      cout << it->first << " " << food_items[it->first].get_name() << endl;
      report << it->first << " " << food_items[it->first].get_name() << endl;
    }
  }

  //for (map<string,int>::iterator wit = well_stocked.begin(); wit != well_stocked.end(); ++wit)
  //  {
  //     if(wit->second >=1 )
  //	cout << wit->first << " " << food_items[wit->first].get_name() <<endl;   
  //  }

  cout << endl;
  report << endl;
  // generate popularity report
  cout << "Most Popular Products:\n";
  report << "Most Popular Products:\n";

  priority_queue<pair<string, long long>,
		 vector<pair<string, long long> >,
		 Comparator> pq;
  long long min = -1;

  for (map<string, long long>::iterator it = popularity.begin();
       it != popularity.end(); ++it)
  {
    pq.push(*it);
  }

  int size = pq.size();

  for (int i = 0; i < 3 && i < size; i++)
  {
    pair<string, long long> p = pq.top();
    cout << p.second << " "
	 << p.first << " "
	 << food_items[p.first].get_name()
	 << endl;

    report << p.second << " "
	 << p.first << " "
	 << food_items[p.first].get_name()
	 << endl;
    pq.pop();
  }

  in.close();
  report.close();
  return 0;
}

// Process all enqueued receve and request orders
void process_orders(
		    queue<order> & receive_orders,
		    queue<order> & request_orders,
		    map<string, map<string, map<date, long long> > > & warehouses,
		    map<string, food> & food_items,
		    map<string, long long> & popularity,
		    map<date, set<string> > & underfilled,
		    date current_date)
{
  // process receive orders
  while (receive_orders.size() > 0)
  {
    order o = receive_orders.front();
    receive(o, warehouses, food_items, current_date);
    receive_orders.pop();
  }

  // process request orders
  while (request_orders.size() > 0)
  {
    request(request_orders.front(), warehouses, popularity, underfilled, current_date);
    request_orders.pop();
  }
}

// Processes a receive order
void receive(order o,
	     map<string, map<string, map<date, long long > > > & warehouses, 
	     map<string, food> & food_items,
	     date current_date)
{
  date expiration;

  food item = food_items[o.upc];
  date_duration dd(item.get_shelf_life());
  expiration = current_date + dd;     

  warehouses[o.location][o.upc][expiration] += o.qty;
}

// Processes a request order
void request(order o, 
	     map<string, map<string, map<date, long long > > > & warehouses,
	     map<string, long long> & popularity,
	     map<date, set<string> > & underfilled,
	     date current_date)
{
  string upc = o.upc;
  string location = o.location;

  // the amount of the order remaining left to fill
  long long to_fill = o.qty;

  // total request amount
  long long request_amt = o.qty;
  

  popularity[upc] += request_amt;

  to_fill = request_amt;      

  // iterate through warehouse inventory of the food item until:
  //   inventory is exhausted
  //   OR
  //   order is completely filled
  for (map<date, long long>::iterator it = warehouses[location][upc].begin();
       it != warehouses[location][upc].end() && to_fill > 0; ++it)
  {
    long long stocked_qty = it->second;

    // underfill case
    if (stocked_qty - to_fill < 0)
    {
      long long temp = stocked_qty;
      stocked_qty = 0;
      to_fill -= temp;
      it->second = stocked_qty;
    }  
    else
    {
      long long temp = stocked_qty;
      stocked_qty -= to_fill;
      to_fill -= temp;
      it->second = stocked_qty;
    }
  }

  // total order is underfilled
  if (to_fill > 0)
  { 
    underfilled[current_date].insert(upc);
  }
}
