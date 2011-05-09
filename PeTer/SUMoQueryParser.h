#ifndef SUMOQUERYPARSER_H
#define SUMOQUERYPARSER_H

#include <map>
#include <string>
#include "../PetriEngine/PQL/PQLParser.h"

std::map<std::string, PetriEngine::PQL::SUMoQuery> ParseSUMoQueries(std::string filename);

#endif // SUMOQUERYPARSER_H
