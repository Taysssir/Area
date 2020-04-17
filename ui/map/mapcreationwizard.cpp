#include <cmath>

#include "mapcreationwizard.h"
#include "ui_mapcreationwizard.h"

MapCreationWizard::MapCreationWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::MapCreationWizard),
    waitingForNewMap_(false),
    mapSize_(8192),
    mapResolution_(0.03)
{
    ui->setupUi(this);

    // Style the map recovery progress bar
    setStyleSheet("QProgressBar{ border: 1px solid grey; border-radius: 3px; text-align: center; font: bold; }");

    connect( this, SIGNAL(currentIdChanged(int)), this, SLOT(on_currentIdChanged(int)) );

    // Handle wizard cancellation
    disconnect( button( QWizard::CancelButton ), SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( button( QWizard::CancelButton ), SIGNAL( clicked() ), this, SLOT( cancelWizard() ) );

    // Handle custom size and resolution value changes
    connect ( ui->spinBoxSize, SIGNAL(valueChanged(int)), this, SLOT(on_customSizeChanged(int)) );

    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );
}

MapCreationWizard::~MapCreationWizard()
{
    delete ui;
}

void MapCreationWizard::setBusyMapRecoveryProgressBar(bool busy)
{
    if (busy)
        ui->mapRecoveryProgressBar->setMaximum(0);
    else
        ui->mapRecoveryProgressBar->setMaximum(100);
}

void MapCreationWizard::updateMapRecoveryProgressBarValue(int iValue)
{
    ui->mapRecoveryProgressBar->setValue(iValue);

    if (iValue == 100)
    {
        waitingForNewMap_ = false;

        // Enable finish button
        QWizard::button(QWizard::FinishButton)->setEnabled(true);
        // Hide Cancel button
        QWizard::button(QWizard::CancelButton)->hide();
    }
}

bool MapCreationWizard::isWaitingForNewMap()
{
    return waitingForNewMap_;
}

void MapCreationWizard::hideEvent(QHideEvent *hideEvent)
{
    if (waitingForNewMap_)
    {
        hideEvent->ignore();
    }
    else
    {
        this->close();
        hideEvent->accept();
    }
}

void MapCreationWizard::closeEvent(QCloseEvent *closeEvent)
{
    if (waitingForNewMap_)
    {
        closeEvent->ignore();
    }
    else
    {
        setBusyMapRecoveryProgressBar(true);
        closeEvent->accept();
    }
}

void MapCreationWizard::keyPressEvent(QKeyEvent *keyEvent)
{
    if (keyEvent->key() == Qt::Key_Escape)
        keyEvent->ignore();
    else
        keyEvent->accept();
}

void MapCreationWizard::setupMapSizeAndResolution()
{
    if (ui->sizeResRadioButton_1->isChecked())
    {
        mapSize_ = 8192;
        mapResolution_ = 0.03;
    }
    else if (ui->sizeResRadioButton_2->isChecked())
    {
        mapSize_ = 8192;
        mapResolution_ = 0.02;
    }
    else if (ui->sizeResRadioButton_3->isChecked())
    {
        mapSize_ = 4096;
        mapResolution_ = 0.03;
    }
    else if (ui->sizeResRadioButton_4->isChecked())
    {
        mapSize_ = 4096;
        mapResolution_ = 0.02;
    }
    else if (ui->sizeResRadioButton_5->isChecked())
    {
        mapResolution_ = ui->doubleSpinBoxResolution->value();
        mapSize_ = roundItUpToMultiple8(ui->spinBoxSize->value() / mapResolution_);
    }
}

void MapCreationWizard::on_currentIdChanged(int id)
{
    Q_UNUSED(id);

    if ( currentPage() == ui->mapSizeAndResolutionPage )
    {
        ui->sizeResRadioButton_1->setChecked(true);
    }
    else if ( currentPage() == ui->robotManipulationPage )
    {
        // Hide back button on this page
        QWizard::button(QWizard::BackButton)->hide();

        // Get selected map size and resolution
        setupMapSizeAndResolution();

        emit createMap(true, mapResolution_, mapSize_);
    }
    else if ( currentPage() == ui->mapRecoveryPage )
    {
        // Set the progress bar to indicate a busy status awaiting recovery percentage
        ui->mapRecoveryProgressBar->setMaximum(0);
        // Disable finish button
        QWizard::button(QWizard::FinishButton)->setEnabled(false);
        // Show Cancel button
        QWizard::button(QWizard::CancelButton)->show();

        emit createMap(false, mapResolution_, mapSize_);

        waitingForNewMap_ = true;
    }
}

void MapCreationWizard::cancelWizard()
{
    if ( currentPage() != ui->mapSizeAndResolutionPage )
    {
        if( QMessageBox::question( this, tr( "Cancel Map Creation" ),
                                   tr( "Are you sure you want to cancel the map creation process?" ),
                                   QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
        {
            waitingForNewMap_ = false;
            emit abortCarto();
            // allow cancel
            reject();
        }
    }
    else // No cancel confirmation needed on first page
    {
        reject();
    }
}

int MapCreationWizard::roundItUpToMultiple8(int iValue)
{
    return 8*((iValue+8/2)/8);
}

void MapCreationWizard::on_customSizeChanged(int iSize)
{
    Q_UNUSED(iSize);
    updateCustomApproxSizes();
}

void MapCreationWizard::updateCustomApproxSizes()
{
    int iSurface = pow((ui->spinBoxSize->value()),2);

    ui->label_custom_mpp->setText( QString(tr(" mpp (≈%1 m²)")).arg(iSurface) );
}
