#ifndef PETRINETSCENEFACTORY_H
#define PETRINETSCENEFACTORY_H

#include "AbstractPetriNetFactory.h"

#include <QUndoGroup>

class PetriNetScene;

/** Factory for PetriNetScene */
class PetriNetSceneFactory : public PetriEngine::AbstractPetriNetFactory
{
	struct ArcEntry{
		QString src;
		QString dst;
		int weight;
	};
public:
	/** undogroup for the scene, and parent for the scene */
	PetriNetSceneFactory(QUndoGroup* undoGroup, QObject* sceneParent);
	void addPlace(const std::string &name, int tokens, double x, double y);
	void addVariable(const std::string& name, int initialValue, int range);
	void addTransition(const std::string &name,
					   const std::string &conditions,
					   const std::string &assignments,
					   double x, double y);
	void addInputArc(const std::string &place, const std::string &transition, int weight);
	void addOutputArc(const std::string &transition, const std::string &place, int weight);
	/** Get the scene created, note you may only call this once */
	PetriNetScene* makeScene();
private:
	PetriNetScene* scene;
	QList<ArcEntry> arcs;
};

#endif // PETRINETSCENEFACTORY_H
