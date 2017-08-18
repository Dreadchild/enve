#include "boxsinglewidget.h"
#include "OptimalScrollArea/singlewidgetabstraction.h"
#include "OptimalScrollArea/singlewidgettarget.h"
#include "OptimalScrollArea/scrollwidgetvisiblepart.h"
#include "Colors/ColorWidgets/colorsettingswidget.h"

#include "Boxes/boxesgroup.h"
#include "qrealanimatorvalueslider.h"
#include "boxscrollwidgetvisiblepart.h"
#include "keysview.h"
#include "pointhelpers.h"
#include "BoxesList/boolpropertywidget.h"
#include "boxtargetwidget.h"
#include "Properties/boxtargetproperty.h"
#include "Animators/qstringanimator.h"

QPixmap *BoxSingleWidget::VISIBLE_PIXMAP;
QPixmap *BoxSingleWidget::INVISIBLE_PIXMAP;
QPixmap *BoxSingleWidget::HIDE_CHILDREN;
QPixmap *BoxSingleWidget::SHOW_CHILDREN;
QPixmap *BoxSingleWidget::LOCKED_PIXMAP;
QPixmap *BoxSingleWidget::UNLOCKED_PIXMAP;
QPixmap *BoxSingleWidget::ANIMATOR_CHILDREN_VISIBLE;
QPixmap *BoxSingleWidget::ANIMATOR_CHILDREN_HIDDEN;
QPixmap *BoxSingleWidget::ANIMATOR_RECORDING;
QPixmap *BoxSingleWidget::ANIMATOR_NOT_RECORDING;
bool BoxSingleWidget::mStaticPixmapsLoaded = false;

#include "global.h"
#include "mainwindow.h"
#include <QInputDialog>
#include <QMenu>
#include "clipboardcontainer.h"
#include "durationrectangle.h"
#include "boxeslistactionbutton.h"

BoxSingleWidget::BoxSingleWidget(ScrollWidgetVisiblePart *parent) :
    SingleWidget(parent) {
    mMainLayout = new QHBoxLayout(this);
    setLayout(mMainLayout);
    mMainLayout->setSpacing(0);
    mMainLayout->setContentsMargins(0, 0, 0, 0);
    mMainLayout->setMargin(0);
    mMainLayout->setAlignment(Qt::AlignLeft);

    mRecordButton = new BoxesListActionButton(this);
    mMainLayout->addWidget(mRecordButton);
    connect(mRecordButton, SIGNAL(pressed()),
            this, SLOT(switchRecordingAction()));

    mContentButton = new BoxesListActionButton(this);
    mMainLayout->addWidget(mContentButton);
    connect(mContentButton, SIGNAL(pressed()),
            this, SLOT(switchContentVisibleAction()));

    mVisibleButton = new BoxesListActionButton(this);
    mMainLayout->addWidget(mVisibleButton);
    connect(mVisibleButton, SIGNAL(pressed()),
            this, SLOT(switchBoxVisibleAction()));

    mLockedButton = new BoxesListActionButton(this);
    mMainLayout->addWidget(mLockedButton);
    connect(mLockedButton, SIGNAL(pressed()),
            this, SLOT(switchBoxLockedAction()));

    mFillWidget = new QWidget(this);
    mMainLayout->addWidget(mFillWidget);
    mFillWidget->setStyleSheet("background-color: rgba(0, 0, 0, 0)");

    mValueSlider = new QrealAnimatorValueSlider(NULL, this);
    mMainLayout->addWidget(mValueSlider, Qt::AlignRight);

    mColorButton = new BoxesListActionButton(this);
    mMainLayout->addWidget(mColorButton, Qt::AlignRight);
    connect(mColorButton, SIGNAL(pressed()),
            this, SLOT(openColorSettingsDialog()));

    mCompositionModeCombo = new QComboBox(this);
    mMainLayout->addWidget(mCompositionModeCombo);
//    mCompositionModeCombo->addItems(QStringList() <<
//                                    "Source Over" <<
//                                    "Destination Over" <<
//                                    "Clear" <<
//                                    "Source" <<
//                                    "Destination" <<
//                                    "Source in" <<
//                                    "Destination in" <<
//                                    "Source Out" <<
//                                    "Destination Out" <<
//                                    "Source Atop" <<
//                                    "Destination Atop" <<
//                                    "Xor" <<
//                                    "Plus" <<
//                                    "Multiply" <<
//                                    "Screen" <<
//                                    "Overlay" <<
//                                    "Darken" <<
//                                    "Lighten" <<
//                                    "Color Burn" <<
//                                    "Hard Light" <<
//                                    "Soft Light" <<
//                                    "Difference" <<
//                                    "Exclusion" <<
//                                    "Source or Destination" <<
//                                    "Source and Destination" <<
//                                    "Source Xor Destination" <<
//                                    "Not Source And Not Destination" <<
//                                    "Not Source or Not Destination" <<
//                                    "Not Source Xor Destination" <<
//                                    "Not Source" <<
//                                    "Not Source And Destination" <<
//                                    "Source And Not Destination" <<
//                                    "Not Source or Destination");
    mCompositionModeCombo->addItems(QStringList() <<
                                    "SrcOver" <<
                                    "DstOver" <<
                                    "SrcIn" <<
                                    "DstIn" <<
                                    "SrcOut" <<
                                    "DstOut" <<
                                    "SrcATop" <<
                                    "DstATop" <<
                                    "Xor" <<
                                    "Plus" <<
                                    "Modulate" <<
                                    "Screen" <<
                                    "Overlay" <<
                                    "Darken" <<
                                    "Lighten" <<
                                    "ColorDodge" <<
                                    "ColorBurn" <<
                                    "HardLight" <<
                                    "SoftLight" <<
                                    "Difference" <<
                                    "Exclusion" <<
                                    "Multiply" <<
                                    "Hue" <<
                                    "Saturation" <<
                                    "Color" <<
                                    "Luminosity");
    mCompositionModeCombo->insertSeparator(10);
    mCompositionModeCombo->insertSeparator(22);
    connect(mCompositionModeCombo, SIGNAL(activated(int)),
            this, SLOT(setCompositionMode(int)));
    mCompositionModeCombo->setSizePolicy(QSizePolicy::Maximum,
                    mCompositionModeCombo->sizePolicy().horizontalPolicy());

    mBoxTargetWidget = new BoxTargetWidget(this);
    mMainLayout->addWidget(mBoxTargetWidget);

    mCheckBox = new BoolPropertyWidget(this);
    mMainLayout->addWidget(mCheckBox);

    mMainLayout->addSpacing(MIN_WIDGET_HEIGHT/2);

    hide();
}

SkBlendMode idToBlendModeSk(const int &id) {
    switch(id) {
        case 0: return SkBlendMode::kSrcOver;
        case 1: return SkBlendMode::kDstOver;
        case 2: return SkBlendMode::kSrcIn;
        case 3: return SkBlendMode::kDstIn;
        case 4: return SkBlendMode::kSrcOut;
        case 5: return SkBlendMode::kDstOut;
        case 6: return SkBlendMode::kSrcATop;
        case 7: return SkBlendMode::kDstATop;
        case 8: return SkBlendMode::kXor;
        case 9: return SkBlendMode::kPlus;
        case 10: return SkBlendMode::kModulate;
        case 11: return SkBlendMode::kScreen;
        case 12: return SkBlendMode::kOverlay;
        case 13: return SkBlendMode::kDarken;
        case 14: return SkBlendMode::kLighten;
        case 15: return SkBlendMode::kColorDodge;
        case 16: return SkBlendMode::kColorBurn;
        case 17: return SkBlendMode::kHardLight;
        case 18: return SkBlendMode::kSoftLight;
        case 19: return SkBlendMode::kDifference;
        case 20: return SkBlendMode::kExclusion;
        case 21: return SkBlendMode::kMultiply;
        case 22: return SkBlendMode::kHue;
        case 23: return SkBlendMode::kSaturation;
        case 24: return SkBlendMode::kColor;
        case 25: return SkBlendMode::kLuminosity;
        default: return SkBlendMode::kSrcOver;
    }
    return SkBlendMode::kSrcOver;
}

int blendModeToIntSk(const SkBlendMode &mode) {
    switch(mode) {
        case SkBlendMode::kSrcOver: return 0;
        case SkBlendMode::kDstOver: return 1;
        case SkBlendMode::kSrcIn: return 2;
        case SkBlendMode::kDstIn: return 3;
        case SkBlendMode::kSrcOut: return 4;
        case SkBlendMode::kDstOut: return 5;
        case SkBlendMode::kSrcATop: return 6;
        case SkBlendMode::kDstATop: return 7;
        case SkBlendMode::kXor: return 8;
        case SkBlendMode::kPlus: return 9;
        case SkBlendMode::kModulate: return 10;
        case SkBlendMode::kScreen: return 11;
        case SkBlendMode::kOverlay: return 12;
        case SkBlendMode::kDarken: return 13;
        case SkBlendMode::kLighten: return 14;
        case SkBlendMode::kColorDodge: return 15;
        case SkBlendMode::kColorBurn: return 16;
        case SkBlendMode::kHardLight: return 17;
        case SkBlendMode::kSoftLight: return 18;
        case SkBlendMode::kDifference: return 19;
        case SkBlendMode::kExclusion: return 20;
        case SkBlendMode::kMultiply: return 21;
        case SkBlendMode::kHue: return 22;
        case SkBlendMode::kSaturation: return 23;
        case SkBlendMode::kColor: return 24;
        case SkBlendMode::kLuminosity: return 25;
        default: return 0;
    }
    return 0;
}

void BoxSingleWidget::setCompositionMode(const int &id) {
    SingleWidgetTarget *target = mTarget->getTarget();

    if(target->SWT_isBoundingBox()) {
        ((BoundingBox*)target)->setBlendModeSk(
                    idToBlendModeSk(id));
    }
    MainWindow::getInstance()->callUpdateSchedulers();
}

void BoxSingleWidget::setBlendMode(const SkBlendMode &mode) {
    int id = blendModeToIntSk(mode);
    mCompositionModeCombo->setCurrentIndex(id);
}

void BoxSingleWidget::setTargetAbstraction(SingleWidgetAbstraction *abs) {
    SingleWidget::setTargetAbstraction(abs);
    SingleWidgetTarget *target = abs->getTarget();

    if(target->SWT_isBoxesGroup()) {
        //BoxesGroup *bg_target = (BoxesGroup*)target;

        //setName(bg_target->getName());

        mRecordButton->hide();

        mContentButton->show();

        mVisibleButton->show();

        mLockedButton->show();

        mColorButton->hide();

        mCompositionModeVisible = true;
        mCompositionModeCombo->setCurrentIndex(
                    ((BoundingBox*)target)->getCompositionMode());
        updateCompositionBoxVisible();

        mBoxTargetWidget->hide();
        mCheckBox->hide();

        mValueSlider->setAnimator(NULL);
        mValueSlider->hide();
    } else if(target->SWT_isBoundingBox()) {
        //BoundingBox *bb_target = (BoundingBox*)target;

        mRecordButton->hide();

        mContentButton->show();

        mVisibleButton->show();

        mLockedButton->show();

        mColorButton->hide();

        mCompositionModeVisible = true;
        mCompositionModeCombo->setCurrentIndex(
            blendModeToIntSk(((BoundingBox*)target)->getBlendMode()) );
        updateCompositionBoxVisible();

        mBoxTargetWidget->hide();
        mCheckBox->hide();

        mValueSlider->setAnimator(NULL);
        mValueSlider->hide();
    } else if(target->SWT_isBoolProperty()) {
        mRecordButton->hide();

        mContentButton->hide();

        mVisibleButton->hide();

        mLockedButton->hide();

        mColorButton->hide();

        mCompositionModeCombo->hide();
        mCompositionModeVisible = false;

        mBoxTargetWidget->hide();

        mCheckBox->show();
        mCheckBox->setTarget((BoolProperty*)target);

        mValueSlider->hide();
    } else if(target->SWT_isQrealAnimator()) {
        QrealAnimator *qa_target = (QrealAnimator*)target;

        mRecordButton->show();

        mContentButton->hide();

        mVisibleButton->hide();

        mLockedButton->hide();

        mColorButton->hide();

        mCompositionModeCombo->hide();
        mCompositionModeVisible = false;

        mBoxTargetWidget->hide();
        mCheckBox->hide();

        mValueSlider->setAnimator(qa_target);
        mValueSlider->show();
    } else if(target->SWT_isComplexAnimator()) {
        //ComplexAnimator *ca_target = (ComplexAnimator*)target;

        mRecordButton->show();

        mContentButton->show();

        mVisibleButton->hide();

        mLockedButton->hide();

        if(target->SWT_isColorAnimator()) {
            mColorButton->show();
        } else {
            mColorButton->hide();
        }

        mCompositionModeCombo->hide();
        mCompositionModeVisible = false;

        mBoxTargetWidget->hide();
        mCheckBox->hide();

        mValueSlider->setAnimator(NULL);
        mValueSlider->hide();
    } else if(target->SWT_isBoxTargetProperty()) {
        mRecordButton->hide();

        mContentButton->hide();

        mVisibleButton->hide();

        mLockedButton->hide();

        mColorButton->hide();

        mCompositionModeCombo->hide();
        mCompositionModeVisible = false;

        mBoxTargetWidget->show();
        mBoxTargetWidget->setTargetProperty((BoxTargetProperty*)target);
        mCheckBox->hide();

        mValueSlider->hide();
    } else if(target->SWT_isQStringAnimator()) {
        mRecordButton->show();

        mContentButton->show();

        mVisibleButton->hide();

        mLockedButton->hide();

        if(target->SWT_isColorAnimator()) {
            mColorButton->show();
        } else {
            mColorButton->hide();
        }

        mCompositionModeCombo->hide();
        mCompositionModeVisible = false;

        mBoxTargetWidget->hide();
        mCheckBox->hide();

        mValueSlider->setAnimator(NULL);
        mValueSlider->hide();
    }
}

void BoxSingleWidget::loadStaticPixmaps() {
    if(mStaticPixmapsLoaded) return;
    VISIBLE_PIXMAP = new QPixmap(":/icons/visible.png");
    INVISIBLE_PIXMAP = new QPixmap(":/icons/hidden.png");
    HIDE_CHILDREN = new QPixmap(":/icons/list_hide_children.png");
    SHOW_CHILDREN = new QPixmap(":/icons/list_show_children.png");
    LOCKED_PIXMAP = new QPixmap(":/icons/lock_locked.png");
    UNLOCKED_PIXMAP = new QPixmap(":/icons/lock_unlocked.png");
    ANIMATOR_CHILDREN_VISIBLE = new QPixmap(
                ":/icons/animator_children_visible.png");
    ANIMATOR_CHILDREN_HIDDEN = new QPixmap(
                ":/icons/animator_children_hidden.png");
    ANIMATOR_RECORDING = new QPixmap(
                ":/icons/recording.png");
    ANIMATOR_NOT_RECORDING = new QPixmap(
                ":/icons/not_recording.png");
    mStaticPixmapsLoaded = true;
}
#include "canvas.h"
void BoxSingleWidget::mousePressEvent(QMouseEvent *event) {
    SingleWidgetTarget *target = mTarget->getTarget();
    if(event->button() == Qt::RightButton) {
        QMenu menu(this);

        if(target->SWT_isBoundingBox()) {
            BoundingBox *boxTarget = ((BoundingBox*)target);
            menu.addAction("Rename")->
                    setObjectName("swt_rename");
            QAction *durRectAct = menu.addAction("Visibility Range");
            durRectAct->setObjectName("swt_visibility_range");
            durRectAct->setCheckable(true);
            durRectAct->setChecked(
                        boxTarget->hasDurationRectangle());
            QMenu *canvasMenu = menu.addMenu("Canvas");
            boxTarget->addActionsToMenu(canvasMenu);
            boxTarget->getParentCanvas()->addCanvasActionToMenu(canvasMenu);
        } else if(target->SWT_isAnimator()) {
            AnimatorClipboardContainer *clipboard =
                    (AnimatorClipboardContainer*)
                    MainWindow::getInstance()->getClipboardContainer(
                                                CCT_ANIMATOR);
            Animator *animTarget = (Animator*)target;
            if(animTarget->prp_isKeyOnCurrentFrame()) {
                menu.addAction("Delete Key")->setObjectName("swt_delete_key");
            } else {
                menu.addAction("Add Key")->setObjectName("swt_add_key");
            }
            menu.addAction("Copy")->setObjectName("swt_copy");
            if(clipboard != NULL) {
                menu.addAction("Paste")->setObjectName("swt_paste");
            }
            if(target->SWT_isPixmapEffect()) {
                menu.addAction("Delete Effect")->setObjectName("swt_delete_effect");
            }
        }
        QAction *selectedAction = menu.exec(event->globalPos());
        if(selectedAction != NULL) {
            if(selectedAction->objectName() == "swt_rename") {
                rename();
            } else if(selectedAction->objectName() == "swt_visibility_range") {
                BoundingBox *boxTarget = (BoundingBox*)target;
                if(boxTarget->hasDurationRectangle()) {
                    boxTarget->setDurationRectangle(NULL);
                } else {
                    boxTarget->createDurationRectangle();
                }
            } else if(selectedAction->objectName() == "swt_copy") {
                AnimatorClipboardContainer *container =
                        new AnimatorClipboardContainer();
                container->setAnimator((QrealAnimator*)target);
                MainWindow::getInstance()->replaceClipboard(container);
            } else if(selectedAction->objectName() == "swt_paste") {
                AnimatorClipboardContainer *clipboard =
                        (AnimatorClipboardContainer*)
                        MainWindow::getInstance()->getClipboardContainer(
                                                    CCT_ANIMATOR);
                clipboard->paste((QrealAnimator*)target);
            } else if(selectedAction->objectName() == "swt_delete_effect") {
                PixmapEffect *effectTarget = (PixmapEffect*)target;
                effectTarget->getParentEffectAnimators()->
                        getParentBox()->removeEffect(effectTarget);
            } else if(selectedAction->objectName() == "swt_delete_key") {
                Animator *animTarget = (Animator*)target;
                animTarget->anim_deleteCurrentKey();
            } else if(selectedAction->objectName() == "swt_add_key") {
                Animator *animTarget = (Animator*)target;
                animTarget->anim_saveCurrentValueAsKey();
            } else if(target->SWT_isBoundingBox()) {
                BoundingBox *boxTarget = ((BoundingBox*)target);
                if(!boxTarget->getParentCanvas()->
                        handleSelectedCanvasAction(selectedAction) ) {
                    boxTarget->handleSelectedCanvasAction(selectedAction);
                }
            }
        } else {

        }
    } else {
        mDragStartPos = event->pos();
//        if(type == SWT_BoundingBox ||
//           type == SWT_BoxesGroup) {
//            BoundingBox *bb_target = (BoundingBox*)target;
//            bb_target->selectionChangeTriggered(event->modifiers() &
//                                                Qt::ShiftModifier);
//        }
    }
    MainWindow::getInstance()->callUpdateSchedulers();
}

#include <QApplication>
#include <QDrag>
void BoxSingleWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!(event->buttons() & Qt::LeftButton)) {
        return;
    }
    if ((event->pos() - mDragStartPos).manhattanLength()
         < QApplication::startDragDistance()) {
        return;
    }
    QDrag *drag = new QDrag(this);

    QMimeData *mimeData = mTarget->getTarget()->SWT_createMimeData();
    if(mimeData == NULL) return;
    drag->setMimeData(mimeData);

    drag->installEventFilter(MainWindow::getInstance());
    drag->exec(Qt::CopyAction | Qt::MoveAction);
}

void BoxSingleWidget::mouseReleaseEvent(QMouseEvent *event) {
    if(pointToLen(event->pos() - mDragStartPos) > MIN_WIDGET_HEIGHT/2) return;
    SingleWidgetTarget *target = mTarget->getTarget();
    if(target->SWT_isBoundingBox() && !target->SWT_isCanvas()) {
        BoundingBox *bb_target = (BoundingBox*)target;
        bb_target->selectionChangeTriggered(event->modifiers() &
                                            Qt::ShiftModifier);
        MainWindow::getInstance()->callUpdateSchedulers();
    } else if(target->SWT_isQrealAnimator()) {
        QrealAnimator *qa_target = (QrealAnimator*)target;
        KeysView *keysView =
                ((BoxScrollWidgetVisiblePart*)mParent)->getKeysView();
        if(keysView != NULL) {
            if(qa_target->isCurrentAnimator(mParent)) {
                keysView->graphRemoveViewedAnimator(qa_target);
            } else {
                keysView->graphAddViewedAnimator(qa_target);
            }
            MainWindow::getInstance()->callUpdateSchedulers();
        }
    }
}

void BoxSingleWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(e->modifiers() & Qt::ShiftModifier) {
        //mousePressEvent(e);
    } else {
        rename();
        MainWindow::getInstance()->callUpdateSchedulers();
    }
}

void BoxSingleWidget::rename() {
    SingleWidgetTarget *target = mTarget->getTarget();
    if(target->SWT_isBoundingBox()) {
        BoundingBox *bb_target = (BoundingBox*)target;
        bool ok;
        QString text = QInputDialog::getText(this, tr("New name dialog"),
                                             tr("Name:"), QLineEdit::Normal,
                                             bb_target->getName(), &ok);
        if(ok) {
            bb_target->setName(text);

            bb_target->
                    SWT_scheduleWidgetsContentUpdateWithSearchNotEmpty();
        }
    }
}

void BoxSingleWidget::drawKeys(QPainter *p, const qreal &pixelsPerFrame,
                               const int &containerTop,
                               const int &minViewedFrame,
                               const int &maxViewedFrame) {
    if(isHidden()) return;
    SingleWidgetTarget *target = mTarget->getTarget();
    if(target->SWT_isAnimator()) {
        Animator *anim_target = (Animator*)target;
        anim_target->prp_drawKeys(p, pixelsPerFrame,
                            containerTop,
                            minViewedFrame, maxViewedFrame);
    }
}

Key *BoxSingleWidget::getKeyAtPos(const int &pressX,
                                  const qreal &pixelsPerFrame,
                                  const int &minViewedFrame) {
    if(isHidden()) return NULL;
    SingleWidgetTarget *target = mTarget->getTarget();
    if(target->SWT_isAnimator()) {
        Animator *anim_target = (Animator*)target;
        return anim_target->prp_getKeyAtPos(pressX,
                               minViewedFrame,
                               pixelsPerFrame);
    }
    return NULL;
}

DurationRectangleMovable *BoxSingleWidget::getRectangleMovableAtPos(
                            const int &pressX,
                            const qreal &pixelsPerFrame,
                            const int &minViewedFrame) {
    if(isHidden()) return NULL;
    SingleWidgetTarget *target = mTarget->getTarget();
    if(target->SWT_isAnimator()) {
        Animator *anim_target = (Animator*)target;
        return anim_target->anim_getRectangleMovableAtPos(
                                    pressX,
                                    minViewedFrame,
                                    pixelsPerFrame);
    }
    return NULL;
}

void BoxSingleWidget::getKeysInRect(const QRectF &selectionRect,
                                    const qreal &pixelsPerFrame,
                                    QList<Key *> *listKeys) {
    if(isHidden()) return;
    SingleWidgetTarget *target = mTarget->getTarget();
    if(target->SWT_isAnimator()) {
        Animator *anim_target = (Animator*)target;

        anim_target->prp_getKeysInRect(selectionRect,
                                 pixelsPerFrame,
                                 listKeys);
    }
}

void drawPixmapCentered(QPainter *p,
                        const QRect &boundingRect,
                        const QPixmap &pixmap) {
    int widthDiff = boundingRect.width() - pixmap.width();
    int heightDiff = boundingRect.height() - pixmap.height();
    int x = widthDiff/2 + boundingRect.x();
    int y = heightDiff/2 + boundingRect.y();
    p->drawPixmap(x, y, pixmap);
}

#include "keysview.h"
void BoxSingleWidget::paintEvent(QPaintEvent *) {
    if(mTarget == NULL) return;
    QPainter p(this);
    SingleWidgetTarget *target = mTarget->getTarget();

    int nameX = mFillWidget->x();
    QString name;
    if(target->SWT_isBoundingBox()) {
        BoundingBox *bb_target = (BoundingBox*)target;

        nameX += MIN_WIDGET_HEIGHT/4;
        name = bb_target->getName();

        p.fillRect(rect(), QColor(0, 0, 0, 50));

        if(mTarget->contentVisible()) {
            drawPixmapCentered(&p, mContentButton->geometry(),
                               *BoxSingleWidget::HIDE_CHILDREN);
        } else {
            drawPixmapCentered(&p, mContentButton->geometry(),
                               *BoxSingleWidget::SHOW_CHILDREN);
        }

        if(bb_target->isVisible()) {
            drawPixmapCentered(&p, mVisibleButton->geometry(),
                               *BoxSingleWidget::VISIBLE_PIXMAP);
        } else {
            drawPixmapCentered(&p, mVisibleButton->geometry(),
                               *BoxSingleWidget::INVISIBLE_PIXMAP);
        }

        if(bb_target->isLocked()) {
            drawPixmapCentered(&p, mLockedButton->geometry(),
                               *BoxSingleWidget::LOCKED_PIXMAP);
        } else {
            drawPixmapCentered(&p, mLockedButton->geometry(),
                               *BoxSingleWidget::UNLOCKED_PIXMAP);
        }

        if(bb_target->isSelected()) {
            p.fillRect(mFillWidget->geometry(),
                       QColor(180, 180, 180));
            p.setPen(Qt::black);
        } else {
            p.setPen(Qt::white);
        }
//        QFont font = p.font();
//        font.setBold(true);
//        p.setFont(font);
    } /*else if(type == SWT_BoxesGroup) {
    } */else if(target->SWT_isQrealAnimator()) {
        QrealAnimator *qa_target = (QrealAnimator*)target;
        if(qa_target->isCurrentAnimator(mParent)) {
            p.fillRect(nameX + MIN_WIDGET_HEIGHT/4, MIN_WIDGET_HEIGHT/4,
                       MIN_WIDGET_HEIGHT/2, MIN_WIDGET_HEIGHT/2,
                       qa_target->getAnimatorColor(mParent));
        }
        name = qa_target->prp_getName();
        nameX += MIN_WIDGET_HEIGHT;
        if(qa_target->prp_isRecording()) {
            drawPixmapCentered(&p, mRecordButton->geometry(),
                               *BoxSingleWidget::ANIMATOR_RECORDING);
        } else {
            drawPixmapCentered(&p, mRecordButton->geometry(),
                               *BoxSingleWidget::ANIMATOR_NOT_RECORDING);
        }

        p.setPen(Qt::white);
    } else if(target->SWT_isComplexAnimator()) {
        ComplexAnimator *ca_target = (ComplexAnimator*)target;
        name = ca_target->prp_getName();

        if(ca_target->prp_isRecording()) {
            drawPixmapCentered(&p, mRecordButton->geometry(),
                               *BoxSingleWidget::ANIMATOR_RECORDING);
        } else {
            drawPixmapCentered(&p, mRecordButton->geometry(),
                               *BoxSingleWidget::ANIMATOR_NOT_RECORDING);
            if(ca_target->prp_isDescendantRecording()) {
                p.save();
                p.setRenderHint(QPainter::Antialiasing);
                p.setBrush(Qt::red);
                p.setPen(Qt::NoPen);
                p.drawEllipse(QPointF(MIN_WIDGET_HEIGHT/2,
                                      MIN_WIDGET_HEIGHT/2),
                              0.125*MIN_WIDGET_HEIGHT,
                              0.125*MIN_WIDGET_HEIGHT);
                p.restore();
            }
        }

        if(mTarget->contentVisible()) {
            drawPixmapCentered(&p, mContentButton->geometry(),
                               *BoxSingleWidget::ANIMATOR_CHILDREN_VISIBLE);
        } else {
            drawPixmapCentered(&p, mContentButton->geometry(),
                               *BoxSingleWidget::ANIMATOR_CHILDREN_HIDDEN);
        }
        p.setPen(Qt::white);

        if(target->SWT_isColorAnimator()) {
            ColorAnimator *col_target = (ColorAnimator*)ca_target;
            p.setBrush(col_target->getCurrentColor().qcol);
            p.drawRect(mColorButton->x(), 3,
                       MIN_WIDGET_HEIGHT, MIN_WIDGET_HEIGHT - 6);
        }
    } else if(target->SWT_isQStringAnimator()) {
        QStringAnimator *ca_target = (QStringAnimator*)target;
        name = ca_target->prp_getName();

        if(ca_target->prp_isRecording()) {
            drawPixmapCentered(&p, mRecordButton->geometry(),
                               *BoxSingleWidget::ANIMATOR_RECORDING);
        } else {
            drawPixmapCentered(&p, mRecordButton->geometry(),
                               *BoxSingleWidget::ANIMATOR_NOT_RECORDING);
        }
     } else if(target->SWT_isBoxTargetProperty()) {
        nameX += 40;
        name = ((BoxTargetProperty*)target)->prp_getName();
    } else if(target->SWT_isBoolProperty()) {
        nameX += 2*MIN_WIDGET_HEIGHT;
        name = ((BoolProperty*)target)->prp_getName();
    }
    p.drawText(QRect(nameX, 0,
                     width() - nameX -
                     MIN_WIDGET_HEIGHT,
                     MIN_WIDGET_HEIGHT),
               name, QTextOption(Qt::AlignVCenter));

    p.end();
}

void BoxSingleWidget::switchContentVisibleAction() {
    mTarget->switchContentVisible();
    MainWindow::getInstance()->callUpdateSchedulers();
    //mParent->callUpdaters();
}

void BoxSingleWidget::switchRecordingAction() {
    ((Animator*)mTarget->getTarget())->prp_switchRecording();
    MainWindow::getInstance()->callUpdateSchedulers();
    update();
}

void BoxSingleWidget::switchBoxVisibleAction() {
    ((BoundingBox*)mTarget->getTarget())->switchVisible();
    MainWindow::getInstance()->callUpdateSchedulers();
    update();
}

void BoxSingleWidget::switchBoxLockedAction() {
    ((BoundingBox*)mTarget->getTarget())->switchLocked();
    MainWindow::getInstance()->callUpdateSchedulers();
    update();
}

void BoxSingleWidget::openColorSettingsDialog() {
    QDialog *dialog = new QDialog(this);
    dialog->setLayout(new QVBoxLayout(dialog));
    ColorSettingsWidget *colorSettingsWidget =
            new ColorSettingsWidget(dialog);
    colorSettingsWidget->setColorAnimatorTarget(
                (ColorAnimator*)mTarget->getTarget());
    dialog->layout()->addWidget(colorSettingsWidget);
    connect(MainWindow::getInstance(), SIGNAL(updateAll()),
            dialog, SLOT(update()));

    dialog->show();
}

void BoxSingleWidget::updateCompositionBoxVisible() {
    if(mCompositionModeVisible) {
        if(width() > 500) {
            mCompositionModeCombo->show();
        } else {
            mCompositionModeCombo->hide();
        }
    }
}

void BoxSingleWidget::resizeEvent(QResizeEvent *) {
    updateCompositionBoxVisible();
}
