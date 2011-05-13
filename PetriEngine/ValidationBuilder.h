/* PeTe - Petri Engine exTremE
 * Copyright (C) 2011  Jonas Finnemann Jensen <jopsen@gmail.com>,
 *                     Thomas Søndersø Nielsen <primogens@gmail.com>,
 *                     Lars Kærlund Østergaard <larsko@gmail.com>,
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef VALIDATIONBUILDER_H
#define VALIDATIONBUILDER_H

#include "AbstractPetriNetBuilder.h"
#include "PQL/PQL.h"

#include <string>
#include <vector>

namespace PetriEngine{

class ValidationBuilder;

/** Error experienced during Validation */
class ValidationError{
	/** Create error for an arc */
	ValidationError(const std::string& startId,
					const std::string& endId,
					const std::string& text)
	 : _startId(startId), _endId(endId), _text(text) {
		_hasExprError = false;
		_isConditionError = false;
	}

	/** Create error for a PQL expression */
	ValidationError(const std::string& id,
					const PQL::ExprError& error,
					bool isConditionError)
	 : _startId(id), _exprError(error) {
		_hasExprError = true;
		_isConditionError = isConditionError;
	}

	/** Create error for an entity */
	ValidationError(const std::string& id,
					const std::string& text)
	 : _startId(id), _text(text) {
		_hasExprError = false;
		_isConditionError = false;
	}
public:
	/** Identifier related to the error, start identifier if related to an arc */
	const std::string& startIdentifier() const { return _startId; }

	/** End identifier to the error if related to an arc, otherwise empty */
	const std::string& endIdentifier() const { return _endId; }

	/** True, if this error carries and non-empty PQL::ExprError */
	bool hasExprError() const { return _hasExprError; }

	/** True, if this error carries an non-empty PQL::ExprError
	 * related to a condition, false if it's related to AssignmentExpression
	 * or carries an empty PQL::ExprError. See also hasExprError().
	 */
	bool isConditionError() const {return _isConditionError; }

	/** Expression error, found in an expression */
	const PQL::ExprError& exprError() const { return _exprError; }

	/** Explanation of the error, empty of it's carries an PQL::ExprError */
	const std::string& text() const { return _text; }
private:
	std::string _startId, _endId;
	bool _hasExprError;
	bool _isConditionError;
	PQL::ExprError _exprError;
	std::string _text;
	friend class ValidationBuilder;
};

/** Builder for validating the consistency of Petri Nets
 * @remarks This builder ensures the following properties:
 *		- Identifiers are unique
 *		- Conditions can be parsed and analysed without problems
 *		- Assignments can be parsed and analysed without problems
 *		- Initial value of variables is within range
 *		- The inital marking isn't negative
 *		- No weights are negative
 *		- Arcs goes between existing start and end points of correct type
 */
class ValidationBuilder : public AbstractPetriNetBuilder
{
	/** Arc, just internal stuff */
	struct Arc{
		Arc(const std::string& start,
			const std::string& end){
			this->start = start;
			this->end = end;
		}
		std::string start;
		std::string end;
	};
public:
    ValidationBuilder();
	void addVariable(const std::string& name,
					 int initialValue,
					 int range);
	void addPlace(const std::string& name,
				  int tokens,
				  double x,
				  double y);
	void addTransition(const std::string& name,
					   const std::string& conditions,
					   const std::string& assignments,
					   double x,
					   double y);
	void addInputArc(const std::string& place,
					 const std::string& transition,
					 int weight);
	void addOutputArc(const std::string& transition,
					  const std::string& place,
					  int weight);
	/** Stop accepting stuff, and start validating
	 * @return true, if no errors were found
	 */
	bool validate();

	/** Get errors found during evaluation */
	const std::vector<ValidationError>& errors() const { return _errors; }
private:
	/** Get the number of matching identifiers */
	int countMatchingIds(const std::string& id);
	std::vector<ValidationError> _errors;
	std::vector<std::string> _varNames;
	std::vector<std::string> _placeNames;
	std::vector<std::string> _transitionNames;
	std::vector<std::string> _conditions;
	std::vector<std::string> _assignments;
	std::vector<Arc> _inputArcs;
	std::vector<Arc> _outputArcs;
};

} // PetriEngine

#endif // VALIDATIONBUILDER_H

