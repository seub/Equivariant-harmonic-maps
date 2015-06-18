#ifndef FENCHELNIELSENUSER_H
#define FENCHELNIELSENUSER_H

#include <QWidget>
#include <QGroupBox>

#include "canvascontainer.h"
#include "equivariantharmonicmapsfactory.h"

class ActionHandler; class QLabel; class QDoubleSpinBox; class QPushButton; class QGridLayout; class Canvas;

class FNselector; class FNmenu; class H2CanvasDelegate;

class FenchelNielsenUser : public QWidget, public CanvasContainer
{
    Q_OBJECT

    friend class ActionHandler;

public:
    FenchelNielsenUser(ActionHandler *handler, int genus);
    void resizeEvent(QResizeEvent *event);

public slots:
    void saveCoordinates();
    void discard();
    void refresh();
    void closeEvent(QCloseEvent *);

private:
    void createWindow();
    void createFactory();
    void canvasResized();
    int menuWidth() const;
    void optimalSize(unsigned int &outputWidth, unsigned int &outputHeight) const;


    ActionHandler *handler;

    QGridLayout* layout;
    FNselector* selector;
    FNmenu* menu;
    Canvas *canvas;
    H2CanvasDelegate *delegate;

    bool saveFNcoordinates;

    EquivariantHarmonicMapsFactory factory;
    int N;
};

class FNselector : public QGroupBox
{
    Q_OBJECT

    friend class FenchelNielsenUser;

public:
    int maxFirstColWidth() const;
    int maxSecondColWidth() const;
    int maxThirdColWidth() const;
    int maxFourthColWidth() const;
    int maxWidth() const;
    int maxHeight() const;

    void getFNcoordinates(std::vector<double> &lengthsOut, std::vector<double> &twistsOut) const;

private:
    FNselector(FenchelNielsenUser *user, int N);

    void resizeEvent(QResizeEvent *);

    void createLayout();
    void createButtons();
    void resetRhos();

    FenchelNielsenUser* user;
    QGridLayout* layout;
    std::vector<QLabel*> lengthsLabels, twistsLabels;
    std::vector<QDoubleSpinBox*> lengthsBoxes, twistsBoxes;

    int N;
    int vertSpace;
    int buttonHeight;
};

class FNmenu : public QGroupBox
{
    Q_OBJECT

    friend class FenchelNielsenUser;

public:
    int maxWidth() const;
    int maxHeight() const;

private:
    FNmenu(FenchelNielsenUser *user);

    void resizeEvent(QResizeEvent *);

    void createLayout();
    void createButtons();

    QGridLayout* layout;
    QPushButton *setButton, *discardButton;
    FenchelNielsenUser *user;

    int vertSpace;
    int buttonHeight;
};


#endif // FENCHELNIELSENUSER_H
