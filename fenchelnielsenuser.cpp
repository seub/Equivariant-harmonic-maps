#include "fenchelnielsenuser.h"

#include <QDoubleSpinBox>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QApplication>
#include <QDesktopWidget>
#include <QStyle>
#include <QResizeEvent>

#include "canvas.h"
#include "actionhandler.h"
#include "canvasdelegate.h"
#include "h2canvasdelegate.h"

FenchelNielsenUser::FenchelNielsenUser(ActionHandler *handler, int genus) : handler(handler)
{
    N = 3*genus - 3;
    saveFNcoordinates = false;

    createWindow();
    createFactory();
}

void FenchelNielsenUser::createWindow()
{
    canvas = new Canvas(this);
    canvas->setEnabled(true);

    selector = new FNselector(this, N);
    selector->setEnabled(true);

    QString message = QString("NB: The canvas shows the fundamental domain and initial triangulation ")
            .append(QString(tr("that would be chosen for a <i> domain </i> representation")));
    infoLabel = new QLabel(message);
    infoLabel->setWordWrap(true);
    infoLabel->setVisible(true);
    infoLabel->setEnabled(true);

    menu = new FNmenu(this, selector->sizeHint().width());
    menu->setEnabled(true);

    layout = new QGridLayout;
    layout->setMargin(9);
    layout->setSpacing(6);

    //layout->setColumnMinimumWidth(0, menuWidth());

    vertSpace = 20;
    layout->setRowMinimumHeight(1, vertSpace);
    layout->addWidget(canvas, 0, 1, 4, 1);
    layout->addWidget(selector, 0, 0, Qt::AlignTop);
    layout->addWidget(infoLabel, 2, 0, Qt::AlignTop);
    layout->addWidget(menu, 3, 0, Qt::AlignTop);
    setLayout(layout);

    setWindowTitle("Fenchel-Nielsen coordinates selector");

    unsigned int width, height;
    optimalSize(width, height);
    resize(width, height);

    move(QApplication::desktop()->screen()->rect().center() - rect().center());
}

void FenchelNielsenUser::createFactory()
{
    factory.setGenus((N+3)/3);
    factory.setMeshDepth(0);
    delegate = (H2CanvasDelegate*) canvas->delegate;
    delegate->buffer.setRhoPointer(&(factory.rhoDomain), "blue");
    delegate->buffer.setMeshPointer(&(factory.mesh), "red");
    delegate->setShowTranslates(false, false);
}

void FenchelNielsenUser::optimalSize(unsigned int &outputWidth, unsigned int &outputHeight) const
{
    unsigned int screenWidth = QApplication::desktop()->width();
    unsigned int screenHeight = QApplication::desktop()->height();

    int canvasOptimalSize = 0.7*screenHeight;

    outputHeight = std::max(selector->maxHeight() + menu->maxHeight() + infoLabel->sizeHint().height() + vertSpace
                            + 2*layout->verticalSpacing(), canvasOptimalSize) + layout->margin();
    if(outputHeight > screenHeight)
    {
        outputHeight = 0.7*(selector->maxHeight() + menu->maxHeight() + infoLabel->sizeHint().height() + vertSpace
                            + 2*layout->verticalSpacing() + layout->margin());
    }
    outputWidth = std::min(selector->sizeHint().width() + 2*layout->margin() + canvasOptimalSize + layout->horizontalSpacing(), Tools::intRound(0.7*screenWidth));
}

int FenchelNielsenUser::menuWidth() const
{
    return std::max(selector->maxWidth(), menu->maxWidth());
}

void FenchelNielsenUser::saveCoordinates()
{
    saveFNcoordinates = true;
    close();
}

void FenchelNielsenUser::discard()
{
    saveFNcoordinates = false;
    close();
}

void FenchelNielsenUser::closeEvent(QCloseEvent *)
{
    if (saveFNcoordinates)
    {
        std::vector<double> lengths, twists;
        selector->getFNcoordinates(lengths, twists);
        handler->receiveFNcoordinates(lengths, twists);
    }
    else
    {
        handler->discardReceiveFNcoordinates();
    }
}

void FenchelNielsenUser::refresh()
{
    std::vector<double> lengths, twists;
    selector->getFNcoordinates(lengths, twists);
    factory.setRhoDomain(lengths, twists);
    delegate->setIsRhoEmpty(false);
    delegate->setIsMeshEmpty(false);
    delegate->buffer.refreshRho();
    delegate->buffer.refreshMesh();
    delegate->refreshTranslates();
    delegate->enableRedrawBuffer(true, true);
    canvas->update();
}

void FenchelNielsenUser::resizeEvent(QResizeEvent * event)
{
    int deltaWidth = abs(event->size().width() - event->oldSize().width());
    int deltaHeight = abs(event->size().height() - event->oldSize().height());

    if (deltaWidth >= 1 || deltaHeight >= 1 || canvas->width()!=canvas->height())
    {
        canvasResized();
    }

    menu->setMinimumWidth(std::max(selector->sizeHint().width(), menu->sizeHint().width()));

    /*std::cout << "selector width = " << selector->width() << std::endl;
    std::cout << "selector hint width = " << selector->sizeHint().width() << std::endl;
    std::cout << "selector maxwidth = " << selector->maxWidth() << std::endl;
    std::cout << "menu width = " << menu->width() << std::endl;
    std::cout << "menu hint width = " << menu->width() << std::endl;
    std::cout << "menu maxWidth = " << menu->maxWidth() << std::endl;
    std::cout << "menuWidth() = " << menuWidth() << std::endl;*/
}

void FenchelNielsenUser::showEvent(QShowEvent *)
{
    refresh();
}

void FenchelNielsenUser::canvasResized()
{
    int margin = layout->margin();
    int extMarginV = std::max(selector->geometry().top(), margin);
    int extMarginH = selector->geometry().left();
    int infoLabelHeight = infoLabel->height() + vertSpace;
    int inputMenuWidth = selector->width();
    int interWidth = layout->horizontalSpacing();
    int interHeight = 0;

    int maxWidthH = (width() - (2*extMarginH + inputMenuWidth + interWidth));
    int maxHeightH = height() - (interHeight + extMarginV + margin);

    int newSizeH, extraWidthH, extraHeightH;
    if (maxWidthH < maxHeightH)
    {
        newSizeH = maxWidthH;
        extraWidthH = 0;
        extraHeightH = std::max(height() - (infoLabelHeight + interHeight + extMarginV + margin) - newSizeH, 0);
    }
    else
    {
        newSizeH = maxHeightH;
        extraHeightH = 0;
        extraWidthH = std::max(width() - (2*extMarginH + inputMenuWidth + 2*interWidth) - newSizeH, 0);
    }

    int wh = extraWidthH/2;
    int hh = extraHeightH/2;

    int leftL, topL, size;
    leftL = (extMarginH + inputMenuWidth + interWidth) + wh;
    topL = extMarginV + hh;
    size = newSizeH;

    QRect canvasRect(leftL, topL, size, size);


    if (canvasRect != canvas->geometry())
    {
        canvas->setGeometry(canvasRect);
        canvas->rescale();
    }
}

FNselector::FNselector(FenchelNielsenUser *user, int N) : user(user), N(N)
{
    setParent(user);
    vertSpace = 5;

    createButtons();
    createLayout();
}

void FNselector::createButtons()
{
    buttonHeight = 30;

    QString numi;
    QDoubleSpinBox *lengthBox, *twistBox;
    QLabel *lengthLabel, *twistLabel;
    for (int i=0; i<N; ++i)
    {
        numi = QString::number(i+1);

        lengthLabel = new QLabel(QString("length %1: ").arg(numi));
        lengthLabel->setFixedHeight(buttonHeight);
        lengthsLabels.push_back(lengthLabel);

        twistLabel = new QLabel(QString("twist %1: ").arg(numi));
        twistLabel->setFixedHeight(buttonHeight);
        twistsLabels.push_back(twistLabel);

        lengthBox = new QDoubleSpinBox();
        lengthBox->setRange(0.10, 8.00);
        lengthBox->setValue(2.00);
        lengthBox->setSingleStep(0.1);
        lengthBox->setFixedHeight(buttonHeight);
        connect(lengthBox, SIGNAL(valueChanged(double)), user, SLOT(refresh()));
        lengthsBoxes.push_back(lengthBox);

        twistBox = new QDoubleSpinBox();
        twistBox->setRange(-12.00, 12.00);
        twistBox->setSingleStep(0.1);
        twistBox->setFixedHeight(buttonHeight);
        connect(twistBox, SIGNAL(valueChanged(double)), user, SLOT(refresh()));
        twistsBoxes.push_back(twistBox);
    }
}

void FNselector::createLayout()
{
    layout = new QGridLayout;
    layout->setSpacing(0);

    for (int i=0; i!=N; ++i)
    {
        layout->setRowMinimumHeight(2*i, vertSpace);
        layout->setRowMinimumHeight(2*i+1, buttonHeight);
    }

    layout->setColumnMinimumWidth(0, maxFirstColWidth());
    //layout->setColumnMinimumWidth(1, maxSecondColWidth());
    layout->setColumnMinimumWidth(2, 4*vertSpace);
    layout->setColumnMinimumWidth(3, maxThirdColWidth());
    //layout->setColumnMinimumWidth(3, maxFourthColWidth());

    setLayout(layout);

    for (int i=0; i!=N; ++i)
    {
        layout->addWidget(lengthsLabels[i], 2*i+1, 0, 1, 1, Qt::AlignRight);
        lengthsLabels[i]->setVisible(true);
        lengthsLabels[i]->setEnabled(true);

        layout->addWidget(lengthsBoxes[i], 2*i+1, 1, 1, 1, Qt::AlignLeft);
        lengthsBoxes[i]->setVisible(true);
        lengthsBoxes[i]->setEnabled(true);

        layout->addWidget(twistsLabels[i], 2*i+1, 3, 1, 1, Qt::AlignRight);
        twistsLabels[i]->setVisible(true);
        twistsLabels[i]->setEnabled(true);

        layout->addWidget(twistsBoxes[i], 2*i+1, 4, 1, 1, Qt::AlignLeft);
        twistsBoxes[i]->setVisible(true);
        twistsBoxes[i]->setEnabled(true);
    }
}

int FNselector::maxFirstColWidth() const
{
    std::vector<unsigned int> widths;
    for (auto item : lengthsLabels)
    {
        widths.push_back(item->sizeHint().width());
    }
    return *std::max_element(widths.begin(), widths.end());
}

int FNselector::maxSecondColWidth() const
{
    std::vector<unsigned int> widths;
    for (auto item : lengthsBoxes)
    {
        widths.push_back(item->sizeHint().width());
    }
    return *std::max_element(widths.begin(), widths.end());
}

int FNselector::maxThirdColWidth() const
{
    std::vector<unsigned int> widths;
    for (auto item : twistsLabels)
    {
        widths.push_back(item->sizeHint().width());
    }
    return *std::max_element(widths.begin(), widths.end());
}

int FNselector::maxFourthColWidth() const
{
    std::vector<unsigned int> widths;
    for (auto item : twistsBoxes)
    {
        widths.push_back(item->sizeHint().width());
    }
    return *std::max_element(widths.begin(), widths.end());
}

int FNselector::maxWidth() const
{
    return maxFirstColWidth() + maxSecondColWidth() + maxThirdColWidth() + maxFourthColWidth() + 4*vertSpace;
}

int FNselector::maxHeight() const
{
    int absurdMargin = 1;
    return QStyle::CE_HeaderLabel + absurdMargin + N*buttonHeight + N*vertSpace;
}

void FNselector::resizeEvent(QResizeEvent *)
{
    if (height() < maxHeight())
    {
        setMinimumHeight(maxHeight());
    }
}

void FNselector::getFNcoordinates(std::vector<double> &lengthsOut, std::vector<double> &twistsOut) const
{
    lengthsOut.clear();
    twistsOut.clear();
    lengthsOut.reserve(N);
    twistsOut.reserve(N);
    for (int i=0; i!=N; ++i)
    {
        lengthsOut.push_back(lengthsBoxes[i]->value());
        twistsOut.push_back(twistsBoxes[i]->value());
    }
}



FNmenu::FNmenu(FenchelNielsenUser *user, int minWidth) : user(user)
{
    setParent(user);
    vertSpace = 5;

    createButtons();
    createLayout();

    setMinimumWidth(minWidth);
}

void FNmenu::createButtons()
{
    setButton = new QPushButton("Proceed");

    buttonHeight = setButton->sizeHint().height();

    setButton->setFixedHeight(buttonHeight);
    setButton->setMaximumWidth(setButton->sizeHint().width());
    connect(setButton, SIGNAL(clicked()), user, SLOT(saveCoordinates()));

    discardButton = new QPushButton("Discard");
    discardButton->setFixedHeight(buttonHeight);
    discardButton->setMaximumWidth(discardButton->sizeHint().width());
    connect(discardButton, SIGNAL(clicked()), user, SLOT(discard()));

    questionLabel = new QLabel("Save these Fenchel-Nielsen coordinates?");
    questionLabel->setWordWrap(true);
}

void FNmenu::createLayout()
{
    layout = new QGridLayout;
    layout->setSpacing(0);

    layout->setRowMinimumHeight(0, 2*vertSpace);
    layout->setRowMinimumHeight(1, 4*vertSpace);
    layout->setRowMinimumHeight(2, 3*vertSpace);
    layout->setRowMinimumHeight(3, buttonHeight);

    //layout->setColumnMinimumWidth(0, maxWidth());
    setLayout(layout);

    layout->addWidget(questionLabel, 1, 0, 1, 2);
    questionLabel->setVisible(true);
    questionLabel->setEnabled(true);

    layout->addWidget(setButton, 3, 0, 1, 1);
    setButton->setVisible(true);
    setButton->setEnabled(true);

    layout->addWidget(discardButton, 3, 1, 1, 1);
    discardButton->setVisible(true);
    discardButton->setEnabled(true);
}

int FNmenu::maxWidth() const
{
    return setButton->sizeHint().width() + discardButton->sizeHint().width();
}

int FNmenu::maxHeight() const
{
    int absurdMargin = 1;
    return QStyle::CE_HeaderLabel + absurdMargin + 2*buttonHeight + 9*vertSpace;
}

void FNmenu::resizeEvent(QResizeEvent *)
{
    if (height() < maxHeight())
    {
        setMinimumHeight(maxHeight());
    }
}


