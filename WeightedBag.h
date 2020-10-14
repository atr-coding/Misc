#ifndef _WEIGHTED_BAG_H
#define _WEIGHTED_BAG_H

#include <vector>
#include <exception>
#include <random>

template <typename T>
class WeightedBag
{
public:
	void addEntry(T item, double weight)
	{
		accumulatedWeight += weight;
        	Entry<T> e {accumulatedWeight, item};
		entries.push_back(e);
	}

	T getRandom()
	{
		double r = (double)(rand() % 100) / 100.0 * accumulatedWeight;

		for (auto e : entries) {
			if (e.accumulatedWeight >= r) {
				return e.item;
			}
		}

		try {
			return entries.at(0).item;
		} catch(std::out_of_range e) {
			throw std::out_of_range("Cannot get element from bag because it is empty.");
		}
	}
private:
	template<typename P>
	struct Entry
    	{
        	double accumulatedWeight;
        	P item;
    	};

	std::vector<Entry<T>> entries;
	double accumulatedWeight { 0.0 };
};


#endif
