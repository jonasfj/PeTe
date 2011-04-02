#ifndef VALIDATIONISSUESMODEL_H
#define VALIDATIONISSUESMODEL_H

#include <QAbstractTableModel>

#include <PetriEngine/ValidationBuilder.h>

#include <vector>

class PetriNetScene;

/** Model for presenting validation issues */
class ValidationIssuesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ValidationIssuesModel(QObject *parent = 0);
	/** Set issues */
	void setIssues(const std::vector<PetriEngine::ValidationError>& issues);

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section,
						Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;
	void showValidationIssue(PetriNetScene* scene, const QModelIndex &index);
private:
	std::vector<PetriEngine::ValidationError> _issues;
signals:

public slots:
	/** Clear all issues */
	void clear();
};

#endif // VALIDATIONISSUESMODEL_H
