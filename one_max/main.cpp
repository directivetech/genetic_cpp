#include <iostream>
#include <array>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>

/// @brief Function for std::sort to compare two individual's fitness
/// @param lhs is 1st individual
/// @param rhs is 2nd individual
/// @return True if fitness of 1st individual is greatest of the two, otherwise False
bool sortByFitness(const std::array<short, 1000> &lhs, const std::array<short, 1000> &rhs)
{
    return std::accumulate(lhs.begin(), lhs.end(), 0) > std::accumulate(rhs.begin(), rhs.end(), 0);
}

/// @brief Evaluates the population by ordering in decending fitness
/// @param population is array of individuals to evaluate
void evaluate(std::array<std::array<short, 1000>, 100>& population)
{
    std::sort(population.begin(), population.end(), sortByFitness);
    return;
}

/// @brief Organizes the population into pairs
/// @param population is the population of individuals
/// @param selections is the population organized into pairs
void selection(std::array<std::array<short, 1000>, 100>& population, 
                std::array<std::pair<std::array<short, 1000>, std::array<short, 1000>>, 50>& selections)
{
    auto it = population.begin();

    for(auto &pair : selections)
    {
        std::copy(it->begin(), it->end(), pair.first.begin());
        std::copy(std::next(it,1)->begin(), std::next(it,1)->end(), pair.second.begin());
        it = std::next(it,2);
    }
    return;
}

/// @brief Helper function for std::for_each to print out individual field value
/// @param field is the value to print
void printIndividualField(const short& field)
{
    std::cout << field;
}

/// @brief Mixes the genes of two individuals at a random point
/// @param selections The pairs of individuals to perform crossover on
void crossover(std::array<std::pair<std::array<short, 1000>, std::array<short, 1000>>, 50>& selections)
{
    std::array<short, 1000> temp;
    int crossoverPoint = 0;

    for(auto &pair : selections)
    {
        // Pick a random point in the 1000 fields
        crossoverPoint = rand() % 1000;
        // Copy from that point to the end from second to temp
        std::copy(std::next(pair.second.begin(), crossoverPoint), pair.second.end(), std::next(temp.begin(), crossoverPoint));
        // Copy from that point to the end from first to second
        std::copy(std::next(pair.first.begin(), crossoverPoint), pair.first.end(), std::next(pair.second.begin(), crossoverPoint));
        // Copy temp to first
        std::copy(std::next(temp.begin(), crossoverPoint), temp.end(), std::next(pair.first.begin(), crossoverPoint));
    }
    return;
}

/// @brief Randomly selects individuals out of the population and shuffles their genes
/// @param population is the population to mutate
void mutation(std::array<std::array<short, 1000>, 100>& population)
{
    std::random_device rd;
    std::mt19937 g(rd());

    std::uniform_real_distribution<> dis(0.0, 1.0);

    for(auto &individual : population)
    {
        if(dis(g) < 0.08)
        {
            std::shuffle(individual.begin(), individual.end(), g);
        }
    }
}

int main(int, char**){

    auto start = std::chrono::high_resolution_clock::now();
    
    // Holds our population of 100 1000-short individuals
    std::array<std::array<short, 1000> , 100> population;

    // Populates all individuals with either a 0 or 1 for all 1000 fields
    for(auto &individual : population)
    {
        for(auto &field : individual)
        {
            field = rand() & 1;
        }
    }
    
    evaluate(population);
    
    // Evaluation sorts the population in decending fitness so the first will always be the best
    std::array<short, 1000> &best = population.front();
    std::array<std::pair<std::array<short, 1000>, std::array<short, 1000>>, 50> selections;

    while(std::accumulate(best.begin(), best.end(),0) != 1000)
    {
        std::cout << "Current Best: " << std::accumulate(best.begin(), best.end(), 0) << std:: endl;

        // Selects invididuals in the population and pairs them for crossover operations
        selection(population, selections);

        crossover(selections);

        // Transforms the pairs of selections back into a flat list of the population
        auto individual = population.begin();
        for(const auto &selection : selections)
        {
            std::copy(std::begin(selection.first), std::end(selection.first), individual->begin());
            individual = std::next(individual, 1);
            std::copy(std::begin(selection.second), std::end(selection.second), individual->begin());
            if(individual != population.end())
            {
                individual = std::next(individual, 1);
            }
        }

        mutation(population);

        evaluate(population);

        best = population.front();
    }

    auto stop = std::chrono::high_resolution_clock::now();

    // Print out the final results of the algorithm
    std::cout << "Best: " << std::accumulate(best.begin(), best.end(), 0) << std:: endl;
    
    std::cout << "Individual: ";
    std::for_each(std::begin(best), std::end(best), printIndividualField);
    
    std::cout << std::endl;
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    
    std::cout << "Completed in " << duration.count() << "ms" << std::endl;
}


