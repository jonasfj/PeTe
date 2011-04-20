#ifndef AUTOARRANGENETCOMMAND_H
#define AUTOARRANGENETCOMMAND_H

#include <QUndoCommand>
#include "../NetItems/PetriNetScene.h"
#include <PetriEngine/AbstractPetriNetBuilder.h>

/** Auto arrange command */
class AutoArrangeNetCommand : public QUndoCommand
{
public:
	AutoArrangeNetCommand(PetriNetScene* scene);
	void redo();
	void undo();

	/** Auxiliary class of building a list of NetItems and their positions */
	class NetItemPositionListBuilder : public PetriEngine::AbstractPetriNetBuilder{
	public:
		NetItemPositionListBuilder(PetriNetScene* scene) {
			this->scene = scene;
		}
		/** Representation of item and position */
		struct NetItemPosition{
			NetItem* item;
			double x, y;
		};
		void addVariable(const std::string &name, int initialValue, int range);
		void addPlace(const std::string &name, int tokens, double x, double y);
		void addTransition(const std::string &name, const std::string &conditions, const std::string &assignments, double x, double y);
		void addInputArc(const std::string &place, const std::string &transition, int weight);
		void addOutputArc(const std::string &transition, const std::string &place, int weight);
		/** Output the final NetItemPositionList */
		const QList<NetItemPosition>& makeNetItemPositionList() const { return list; }
	private:
		PetriNetScene* scene;
		QList<NetItemPosition> list;
	};
private:
	PetriNetScene* _scene;
	/** Positions before auto arrange */
	QList<NetItemPositionListBuilder::NetItemPosition> beforeList;
	/** Positions after auto arrange */
	QList<NetItemPositionListBuilder::NetItemPosition> afterList;
};

#endif // AUTOARRANGENETCOMMAND_H
