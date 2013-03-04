#ifndef CDF_BSEARCH_H
#define CDF_BSEARCH_H

#include <vector>
#include <valarray>

template<class T>
unsigned int cdf_bsearch(T xi, const std::vector<T>& cdf_table)
{
  unsigned int table_size = cdf_table.size();
  unsigned int middle = table_size = table_size>>1;
  while(table_size > 0)
  {
    table_size = table_size>>1;
    if(xi > cdf_table[middle])
    {
      table_size += middle&1;
      middle += table_size;
    }
    else if(xi < cdf_table[middle - 1])
      middle -= table_size + (middle&1);
    else
      break;
  }
  return middle;
}

template<class T>
unsigned int cdf_bsearch(T xi, const std::valarray<T>& cdf_table)
{
  unsigned int table_size = cdf_table.size();
  unsigned int middle = table_size = table_size>>1;
  while(table_size > 0)
  {
    table_size = table_size>>1;
    if(xi > cdf_table[middle])
    {
      table_size += middle&1;
      middle += table_size;
    }
    else if(xi < cdf_table[middle - 1])
      middle -= table_size + (middle&1);
    else
      break;
  }
  return middle;
}

template<class T>
unsigned int cdf_bsearch(T xi, T* cdf_table, unsigned int table_size)
{
  unsigned int middle = table_size = table_size>>1;
  while(table_size > 0)
  {
    table_size = table_size>>1;
    if(xi > cdf_table[middle])
    {
      table_size += middle&1;
      middle += table_size;
    }
    else if(xi < cdf_table[middle - 1])
      middle -= table_size + (middle&1);
    else
      break;
  }
  return middle;
}

#endif // CDF_BSEARCH_H