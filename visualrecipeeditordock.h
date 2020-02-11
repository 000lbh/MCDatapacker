#ifndef VISUALRECIPEEDITORDOCK_H
#define VISUALRECIPEEDITORDOCK_H

#include "mcrinvslot.h"

#include <QDockWidget>
//#include <QVector>

namespace Ui {
class VisualRecipeEditorDock;
}

class VisualRecipeEditorDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit VisualRecipeEditorDock(QWidget *parent = nullptr);
    ~VisualRecipeEditorDock();

    void writeRecipe();
    void readRecipe();

private slots:
    void onRecipeTabChanged(int index);
    //void onRecipeChanged(); // Unused

private:
    Ui::VisualRecipeEditorDock *ui;

    QVector<MCRInvSlot*> craftingSlots;
    int lastTabIndex = 0;
    int lastStackIndex = 0;

    void setupItemList();
    void setupCustomTab();
    void onItemListSearch(const QString &input);
    QJsonObject genCraftingJson(QJsonObject root);
    QJsonObject genSmeltingJson(QJsonObject root);
    QJsonObject genStonecuttingJson(QJsonObject root);
    void readCraftingJson(const QJsonObject &root);
    void readSmeltingJson(const QJsonObject &root);
    void readStonecuttingJson(const QJsonObject &root);

};

#endif // VISUALRECIPEEDITORDOCK_H
