#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/listctrl.h>
#include <wx/grid.h>
#include <wx/spinctrl.h>
#include <wx/filedlg.h>
#include <wx/scrolwin.h>
#include <wx/timer.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <queue>
#include <tuple>
#include <set>

class MainFrame;
class GanttChart;
class TimelineChart;

// Estructuras de datos
struct Process
{
    wxString pid;
    int burstTime;
    int arrivalTime;
    int priority;
    wxColour color;

    int startTime = 0;
    int finishTime = 0;
    int waitingTime = 0;

    std::vector<std::pair<int, int>> segments;
};

struct Resource
{
    wxString name;
    int counter;
};

struct Action
{
    wxString pid;
    wxString action;
    wxString resource;
    int cycle;
};

// Ventana principal
class OSSimulatorApp : public wxApp
{
public:
    bool OnInit() override;
};

// Panel para visualizacion diagrama de Gantt
class GanttChart : public wxScrolledWindow
{
public:
    GanttChart(wxWindow *parent);
    void StartSimulation();
    void StopSimulation();
    void ResetChart();
    void SetProcesses(const std::vector<Process> &processes);

private:
    void OnPaint(wxPaintEvent &event);
    void OnTimer(wxTimerEvent &event);
    void DrawTimeAxis(wxPaintDC &dc);
    void DrawProcessBlocks(wxPaintDC &dc);

    wxTimer *m_timer;
    std::vector<Process> m_processes;
    int m_currentCycle;
    bool m_isRunning;

    wxDECLARE_EVENT_TABLE();
};

// Panel para timeline de sincronizacion
class TimelineChart : public wxScrolledWindow
{
public:
    TimelineChart(wxWindow *parent);
    void StartSimulation();
    void StopSimulation();
    void ResetChart();
    void SetData(const std::vector<Process> &processes,
                 const std::vector<Resource> &resources,
                 const std::vector<Action> &actions);

    // Setter para el modo de sincronización (Mutex Locks o Semaforos)
    void SetSyncMode(const wxString &mode) { m_syncMode = mode; }

private:
    void OnPaint(wxPaintEvent &event);
    void OnTimer(wxTimerEvent &event);
    void DrawTimeline(wxPaintDC &dc);

    wxTimer *m_timer;
    std::vector<Process> m_processes;
    std::vector<Resource> m_resources;
    std::vector<Action> m_actions;
    int m_currentCycle;
    bool m_isRunning;

    std::map<wxString, int> m_resourceStates;
    std::set<std::tuple<wxString, wxString, int>> m_actionsDone;
    std::map<std::tuple<wxString, wxString, int>, int> m_actionStartCycle;

    wxString m_syncMode; // "Mutex Locks" o "Semaforos"
    std::map<wxString, std::queue<int>> m_pendingReleases;

    wxDECLARE_EVENT_TABLE();
};

// Panel de calendarizacion
class SchedulingPanel : public wxPanel
{
public:
    SchedulingPanel(wxWindow *parent);

private:
    void OnLoadProcesses(wxCommandEvent &event);
    void OnStartSimulation(wxCommandEvent &event);
    void OnStopSimulation(wxCommandEvent &event);
    void OnResetSimulation(wxCommandEvent &event);
    void OnAlgorithmCheck(wxCommandEvent &event);
    void OnQuantumChange(wxSpinEvent &event);
    void LoadProcessesFromFile(const wxString &filename);
    void UpdateMetrics();
    void ScheduleFIFO();
    void ScheduleSJF();
    void ScheduleSRT();
    void ScheduleRR();
    void SchedulePriority();

    wxCheckBox *m_fifoCheck;
    wxCheckBox *m_sjfCheck;
    wxCheckBox *m_srtCheck;
    wxCheckBox *m_rrCheck;
    wxCheckBox *m_priorityCheck;
    wxSpinCtrl *m_quantumSpin;
    wxButton *m_loadProcessesBtn;
    wxButton *m_startBtn;
    wxButton *m_stopBtn;
    wxButton *m_resetBtn;
    wxListCtrl *m_processListCtrl;
    wxGrid *m_metricsGrid;
    GanttChart *m_ganttChart;

    std::vector<Process> m_processes;

    wxDECLARE_EVENT_TABLE();
};

// Panel de sincronizacion
class SynchronizationPanel : public wxPanel
{
public:
    SynchronizationPanel(wxWindow *parent);

private:
    void OnLoadProcesses(wxCommandEvent &event);
    void OnLoadResources(wxCommandEvent &event);
    void OnLoadActions(wxCommandEvent &event);
    void OnStartSimulation(wxCommandEvent &event);
    void OnStopSimulation(wxCommandEvent &event);
    void OnResetSimulation(wxCommandEvent &event);
    void OnSyncModeChange(wxCommandEvent &event);
    void LoadProcessesFromFile(const wxString &filename);
    void LoadResourcesFromFile(const wxString &filename);
    void LoadActionsFromFile(const wxString &filename);
    void CheckEnableStart();

    wxChoice *m_syncModeChoice;
    wxButton *m_loadProcessesBtn;
    wxButton *m_loadResourcesBtn;
    wxButton *m_loadActionsBtn;
    wxButton *m_startBtn;
    wxButton *m_stopBtn;
    wxButton *m_resetBtn;
    wxListCtrl *m_processListCtrl;
    wxListCtrl *m_resourceListCtrl;
    wxListCtrl *m_actionListCtrl;
    TimelineChart *m_timelineChart;

    std::vector<Process> m_processes;
    std::vector<Resource> m_resources;
    std::vector<Action> m_actions;

    wxDECLARE_EVENT_TABLE();
};

// Frame principal
class MainFrame : public wxFrame
{
public:
    MainFrame();
    wxNotebook *GetNotebook() { return m_notebook; }

private:
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnNotebookPageChanged(wxBookCtrlEvent &event);

    wxNotebook *m_notebook;
    SchedulingPanel *m_schedulingPanel;
    SynchronizationPanel *m_syncPanel;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
        EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
            EVT_NOTEBOOK_PAGE_CHANGED(wxID_ANY, MainFrame::OnNotebookPageChanged)
                wxEND_EVENT_TABLE()

                    wxBEGIN_EVENT_TABLE(GanttChart, wxScrolledWindow)
                        EVT_PAINT(GanttChart::OnPaint)
                            EVT_TIMER(wxID_ANY, GanttChart::OnTimer)
                                wxEND_EVENT_TABLE()

                                    wxBEGIN_EVENT_TABLE(TimelineChart, wxScrolledWindow)
                                        EVT_PAINT(TimelineChart::OnPaint)
                                            EVT_TIMER(wxID_ANY, TimelineChart::OnTimer)
                                                wxEND_EVENT_TABLE()

                                                    wxBEGIN_EVENT_TABLE(SchedulingPanel, wxPanel)
                                                        EVT_BUTTON(1001, SchedulingPanel::OnLoadProcesses)
                                                            EVT_BUTTON(1002, SchedulingPanel::OnStartSimulation)
                                                                EVT_BUTTON(1003, SchedulingPanel::OnStopSimulation)
                                                                    EVT_BUTTON(1004, SchedulingPanel::OnResetSimulation)
                                                                        EVT_CHECKBOX(1010, SchedulingPanel::OnAlgorithmCheck)
                                                                            EVT_CHECKBOX(1011, SchedulingPanel::OnAlgorithmCheck)
                                                                                EVT_CHECKBOX(1012, SchedulingPanel::OnAlgorithmCheck)
                                                                                    EVT_CHECKBOX(1013, SchedulingPanel::OnAlgorithmCheck)
                                                                                        EVT_CHECKBOX(1014, SchedulingPanel::OnAlgorithmCheck)

                                                                                            EVT_SPINCTRL(1005, SchedulingPanel::OnQuantumChange)
                                                                                                wxEND_EVENT_TABLE()

                                                                                                    wxBEGIN_EVENT_TABLE(SynchronizationPanel, wxPanel)
                                                                                                        EVT_BUTTON(2001, SynchronizationPanel::OnLoadProcesses)
                                                                                                            EVT_BUTTON(2002, SynchronizationPanel::OnLoadResources)
                                                                                                                EVT_BUTTON(2003, SynchronizationPanel::OnLoadActions)
                                                                                                                    EVT_BUTTON(2004, SynchronizationPanel::OnStartSimulation)
                                                                                                                        EVT_BUTTON(2005, SynchronizationPanel::OnStopSimulation)
                                                                                                                            EVT_BUTTON(2006, SynchronizationPanel::OnResetSimulation)
                                                                                                                                EVT_CHOICE(2007, SynchronizationPanel::OnSyncModeChange)
                                                                                                                                    wxEND_EVENT_TABLE()

    // Implementaciones

    bool OSSimulatorApp::OnInit()
{
    MainFrame *frame = new MainFrame();
    frame->Show(true);
    return true;
}

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "Simulador de Sistemas Operativos",
                                 wxDefaultPosition, wxSize(1200, 800))
{
    // Menu bar
    wxMenuBar *menuBar = new wxMenuBar;
    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, "E&xit\tCtrl-Q", "Quit this program");

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, "&About\tF1", "Show about dialog");

    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");
    SetMenuBar(menuBar);

    // Status bar
    CreateStatusBar(2);
    SetStatusText("Listo para simular", 0);

    // CORRECCION: Crear notebook principal con estilo explicito
    m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                wxNB_TOP | wxNB_FIXEDWIDTH);

    // Crear paneles
    m_schedulingPanel = new SchedulingPanel(m_notebook);
    m_syncPanel = new SynchronizationPanel(m_notebook);

    // CORRECCION: Agregar paginas con titulos mas claros y asegurar que se muestren
    m_notebook->AddPage(m_schedulingPanel, "A. Calendarizacion de Procesos", true);
    m_notebook->AddPage(m_syncPanel, "B. Sincronizacion de Recursos", false);

    // CORRECCION: Forzar que el notebook sea visible y tenga el tamaño correcto
    m_notebook->SetMinSize(wxSize(1180, 750));

    // Layout principal
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(m_notebook, 1, wxEXPAND | wxALL, 5);
    SetSizer(mainSizer);

    // CORRECCION: Asegurar que el layout se actualice
    Layout();
    Centre();
}

void MainFrame::OnNotebookPageChanged(wxBookCtrlEvent &event)
{
    int selection = event.GetSelection();
    wxString tabName;

    switch (selection)
    {
    case 0:
        tabName = "Calendarizacion de Procesos";
        SetStatusText("Modo: Simulacion de Algoritmos de Scheduling", 1);
        break;
    case 1:
        tabName = "Sincronizacion de Recursos";
        SetStatusText("Modo: Simulacion de Mecanismos de Sincronizacion", 1);
        break;
    default:
        tabName = "Desconocido";
        break;
    }

    SetStatusText("Pestana activa: " + tabName, 0);
}

// Para SchedulingPanel - agregar titulo distintivo
SchedulingPanel::SchedulingPanel(wxWindow *parent) : wxPanel(parent)
{
    // TITULO DEL PANEL
    wxStaticText *titleText = new wxStaticText(this, wxID_ANY,
                                               "A. SIMULADOR DE ALGORITMOS DE CALENDARIZACION",
                                               wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);

    wxFont titleFont = titleText->GetFont();
    titleFont.SetPointSize(12);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    titleText->SetFont(titleFont);
    titleText->SetForegroundColour(wxColour(0, 100, 200));

    // Panel de control superior
    wxStaticBoxSizer *controlBox = new wxStaticBoxSizer(wxHORIZONTAL, this, "Configuracion de Scheduling");

    // Algoritmos
    wxStaticBoxSizer *algBox = new wxStaticBoxSizer(wxVERTICAL, this, "Algoritmos");
    m_fifoCheck = new wxCheckBox(this, 1010, "First In First Out (FIFO)");
    m_sjfCheck = new wxCheckBox(this, 1011, "Shortest Job First (SJF)");
    m_srtCheck = new wxCheckBox(this, 1012, "Shortest Remaining Time (SRT)");
    m_rrCheck = new wxCheckBox(this, 1013, "Round Robin (RR)");
    m_priorityCheck = new wxCheckBox(this, 1014, "Priority");

    algBox->Add(m_fifoCheck, 0, wxALL, 2);
    algBox->Add(m_sjfCheck, 0, wxALL, 2);
    algBox->Add(m_srtCheck, 0, wxALL, 2);
    algBox->Add(m_rrCheck, 0, wxALL, 2);
    algBox->Add(m_priorityCheck, 0, wxALL, 2);

    // Quantum para Round Robin
    wxStaticBoxSizer *quantumBox = new wxStaticBoxSizer(wxVERTICAL, this, "Quantum (RR)");
    m_quantumSpin = new wxSpinCtrl(this, 1005, "3", wxDefaultPosition, wxDefaultSize,
                                   wxSP_ARROW_KEYS, 1, 100, 3);
    quantumBox->Add(new wxStaticText(this, wxID_ANY, "Ciclos:"), 0, wxALL, 2);
    quantumBox->Add(m_quantumSpin, 0, wxALL, 2);

    // Botones de control
    wxStaticBoxSizer *btnBox = new wxStaticBoxSizer(wxVERTICAL, this, "Control");
    m_loadProcessesBtn = new wxButton(this, 1001, "Cargar Procesos");
    m_startBtn = new wxButton(this, 1002, "Iniciar Simulacion");
    m_stopBtn = new wxButton(this, 1003, "Detener");
    m_resetBtn = new wxButton(this, 1004, "Reiniciar");

    m_startBtn->Enable(false);
    m_stopBtn->Enable(false);

    btnBox->Add(m_loadProcessesBtn, 0, wxEXPAND | wxALL, 2);
    btnBox->Add(m_startBtn, 0, wxEXPAND | wxALL, 2);
    btnBox->Add(m_stopBtn, 0, wxEXPAND | wxALL, 2);
    btnBox->Add(m_resetBtn, 0, wxEXPAND | wxALL, 2);

    controlBox->Add(algBox, 1, wxEXPAND | wxALL, 5);
    controlBox->Add(quantumBox, 0, wxEXPAND | wxALL, 5);
    controlBox->Add(btnBox, 0, wxEXPAND | wxALL, 5);

    // Panel de informacion
    wxStaticBoxSizer *infoBox = new wxStaticBoxSizer(wxHORIZONTAL, this, "Informacion");

    // Lista de procesos
    wxStaticBoxSizer *processBox = new wxStaticBoxSizer(wxVERTICAL, this, "Procesos Cargados");
    m_processListCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(300, 150),
                                       wxLC_REPORT | wxLC_SINGLE_SEL);
    m_processListCtrl->AppendColumn("PID", wxLIST_FORMAT_LEFT, 60);
    m_processListCtrl->AppendColumn("BT", wxLIST_FORMAT_RIGHT, 60);
    m_processListCtrl->AppendColumn("AT", wxLIST_FORMAT_RIGHT, 60);
    m_processListCtrl->AppendColumn("Priority", wxLIST_FORMAT_RIGHT, 80);
    processBox->Add(m_processListCtrl, 1, wxEXPAND | wxALL, 2);

    // Metricas
    wxStaticBoxSizer *metricsBox = new wxStaticBoxSizer(wxVERTICAL, this, "Metricas de Eficiencia");
    m_metricsGrid = new wxGrid(this, wxID_ANY, wxDefaultPosition, wxSize(300, 150));
    m_metricsGrid->CreateGrid(3, 2);
    m_metricsGrid->SetColLabelValue(0, "Metrica");
    m_metricsGrid->SetColLabelValue(1, "Valor");
    m_metricsGrid->SetRowLabelValue(0, "1");
    m_metricsGrid->SetRowLabelValue(1, "2");
    m_metricsGrid->SetRowLabelValue(2, "3");
    m_metricsGrid->SetCellValue(0, 0, "Avg Waiting Time");
    m_metricsGrid->SetCellValue(1, 0, "Avg Turnaround Time");
    m_metricsGrid->SetCellValue(2, 0, "Throughput");
    m_metricsGrid->EnableEditing(false);
    metricsBox->Add(m_metricsGrid, 1, wxEXPAND | wxALL, 2);

    infoBox->Add(processBox, 1, wxEXPAND | wxALL, 5);
    infoBox->Add(metricsBox, 1, wxEXPAND | wxALL, 5);

    // Grafico Gantt
    wxStaticBoxSizer *ganttBox = new wxStaticBoxSizer(wxVERTICAL, this, "Diagrama de Gantt");
    m_ganttChart = new GanttChart(this);
    ganttBox->Add(m_ganttChart, 1, wxEXPAND | wxALL, 2);

    // CORRECCION: Agregar titulo al layout principal
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(titleText, 0, wxEXPAND | wxALL, 10);
    mainSizer->Add(controlBox, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(infoBox, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(ganttBox, 1, wxEXPAND | wxALL, 5);

    SetSizer(mainSizer);
}

SynchronizationPanel::SynchronizationPanel(wxWindow *parent) : wxPanel(parent)
{
    // TITULO DEL PANEL
    wxStaticText *titleText = new wxStaticText(this, wxID_ANY,
                                               "B. SIMULADOR DE MECANISMOS DE SINCRONIZACION",
                                               wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);

    wxFont titleFont = titleText->GetFont();
    titleFont.SetPointSize(12);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    titleText->SetFont(titleFont);
    titleText->SetForegroundColour(wxColour(200, 100, 0));

    // Panel de control superior
    wxStaticBoxSizer *controlBox = new wxStaticBoxSizer(wxHORIZONTAL, this, "Configuracion de Sincronizacion");

    // Modo de sincronizacion
    wxStaticBoxSizer *syncBox = new wxStaticBoxSizer(wxVERTICAL, this, "Mecanismo");
    wxArrayString syncChoices;
    syncChoices.Add("Mutex Locks");
    syncChoices.Add("Semaforos");
    m_syncModeChoice = new wxChoice(this, 2007, wxDefaultPosition, wxDefaultSize, syncChoices);
    m_syncModeChoice->SetSelection(0);
    syncBox->Add(new wxStaticText(this, wxID_ANY, "Tipo:"), 0, wxALL, 2);
    syncBox->Add(m_syncModeChoice, 0, wxEXPAND | wxALL, 2);

    // Botones de carga
    wxStaticBoxSizer *loadBox = new wxStaticBoxSizer(wxVERTICAL, this, "Carga de Archivos");
    m_loadProcessesBtn = new wxButton(this, 2001, "Cargar Procesos");
    m_loadResourcesBtn = new wxButton(this, 2002, "Cargar Recursos");
    m_loadActionsBtn = new wxButton(this, 2003, "Cargar Acciones");
    loadBox->Add(m_loadProcessesBtn, 0, wxEXPAND | wxALL, 2);
    loadBox->Add(m_loadResourcesBtn, 0, wxEXPAND | wxALL, 2);
    loadBox->Add(m_loadActionsBtn, 0, wxEXPAND | wxALL, 2);

    // Botones de control
    wxStaticBoxSizer *btnBox = new wxStaticBoxSizer(wxVERTICAL, this, "Control");
    m_startBtn = new wxButton(this, 2004, "Iniciar Simulacion");
    m_stopBtn = new wxButton(this, 2005, "Detener");
    m_resetBtn = new wxButton(this, 2006, "Reiniciar");

    m_startBtn->Enable(false);
    m_stopBtn->Enable(false);

    btnBox->Add(m_startBtn, 0, wxEXPAND | wxALL, 2);
    btnBox->Add(m_stopBtn, 0, wxEXPAND | wxALL, 2);
    btnBox->Add(m_resetBtn, 0, wxEXPAND | wxALL, 2);

    controlBox->Add(syncBox, 1, wxEXPAND | wxALL, 5);
    controlBox->Add(loadBox, 1, wxEXPAND | wxALL, 5);
    controlBox->Add(btnBox, 0, wxEXPAND | wxALL, 5);

    // Panel de informacion
    wxStaticBoxSizer *infoBox = new wxStaticBoxSizer(wxHORIZONTAL, this, "Informacion Cargada");

    // Lista de procesos
    wxStaticBoxSizer *processBox = new wxStaticBoxSizer(wxVERTICAL, this, "Procesos");
    m_processListCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(200, 120),
                                       wxLC_REPORT | wxLC_SINGLE_SEL);
    m_processListCtrl->AppendColumn("PID", wxLIST_FORMAT_LEFT, 60);
    m_processListCtrl->AppendColumn("BT", wxLIST_FORMAT_RIGHT, 50);
    m_processListCtrl->AppendColumn("AT", wxLIST_FORMAT_RIGHT, 50);
    m_processListCtrl->AppendColumn("Priority", wxLIST_FORMAT_RIGHT, 60);
    processBox->Add(m_processListCtrl, 1, wxEXPAND | wxALL, 2);

    // Lista de recursos
    wxStaticBoxSizer *resourceBox = new wxStaticBoxSizer(wxVERTICAL, this, "Recursos");
    m_resourceListCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(200, 120),
                                        wxLC_REPORT | wxLC_SINGLE_SEL);
    m_resourceListCtrl->AppendColumn("Recurso", wxLIST_FORMAT_LEFT, 100);
    m_resourceListCtrl->AppendColumn("Contador", wxLIST_FORMAT_RIGHT, 80);
    resourceBox->Add(m_resourceListCtrl, 1, wxEXPAND | wxALL, 2);

    // Lista de acciones
    wxStaticBoxSizer *actionBox = new wxStaticBoxSizer(wxVERTICAL, this, "Acciones");
    m_actionListCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(250, 120),
                                      wxLC_REPORT | wxLC_SINGLE_SEL);
    m_actionListCtrl->AppendColumn("PID", wxLIST_FORMAT_LEFT, 50);
    m_actionListCtrl->AppendColumn("Accion", wxLIST_FORMAT_LEFT, 70);
    m_actionListCtrl->AppendColumn("Recurso", wxLIST_FORMAT_LEFT, 70);
    m_actionListCtrl->AppendColumn("Ciclo", wxLIST_FORMAT_RIGHT, 50);
    actionBox->Add(m_actionListCtrl, 1, wxEXPAND | wxALL, 2);

    infoBox->Add(processBox, 1, wxEXPAND | wxALL, 5);
    infoBox->Add(resourceBox, 1, wxEXPAND | wxALL, 5);
    infoBox->Add(actionBox, 1, wxEXPAND | wxALL, 5);

    // Timeline
    wxStaticBoxSizer *timelineBox = new wxStaticBoxSizer(wxVERTICAL, this, "Linea de Tiempo");
    m_timelineChart = new TimelineChart(this);
    timelineBox->Add(m_timelineChart, 1, wxEXPAND | wxALL, 2);

    // CORRECCION: Agregar titulo al layout principal
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(titleText, 0, wxEXPAND | wxALL, 10);
    mainSizer->Add(controlBox, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(infoBox, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(timelineBox, 1, wxEXPAND | wxALL, 5);

    SetSizer(mainSizer);
}

GanttChart::GanttChart(wxWindow *parent) : wxScrolledWindow(parent),
                                           m_currentCycle(0), m_isRunning(false)
{
    m_timer = new wxTimer(this);
    SetScrollbars(20, 20, 100, 50);
    SetBackgroundColour(*wxWHITE);
}

TimelineChart::TimelineChart(wxWindow *parent) : wxScrolledWindow(parent),
                                                 m_currentCycle(0), m_isRunning(false)
{
    m_timer = new wxTimer(this);
    SetScrollbars(20, 20, 100, 50);
    SetBackgroundColour(*wxWHITE);
}

void MainFrame::OnExit(wxCommandEvent &event) { Close(true); }
void MainFrame::OnAbout(wxCommandEvent &event)
{
    wxMessageBox("Simulador de Sistemas Operativos\nUniversidad del Valle de Guatemala",
                 "Acerca de", wxOK | wxICON_INFORMATION);
}

void SchedulingPanel::OnLoadProcesses(wxCommandEvent &event)
{
    wxFileDialog dialog(this, "Seleccionar archivo de procesos", "", "",
                        "Archivos de texto (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (dialog.ShowModal() == wxID_OK)
    {
        LoadProcessesFromFile(dialog.GetPath());
    }
}

void SchedulingPanel::OnStartSimulation(wxCommandEvent &event)
{
    if (m_fifoCheck->GetValue())
    {
        ScheduleFIFO();
    }
    else if (m_sjfCheck->GetValue())
    {
        ScheduleSJF();
    }
    else if (m_srtCheck->GetValue())
    {
        ScheduleSRT();
    }
    else if (m_rrCheck->GetValue())
    {
        ScheduleRR();
    }
    else if (m_priorityCheck->GetValue())
    {
        SchedulePriority();
    }

    m_ganttChart->StartSimulation();
    m_startBtn->Enable(false);
    m_stopBtn->Enable(true);
}

void SchedulingPanel::OnStopSimulation(wxCommandEvent &event)
{
    m_ganttChart->StopSimulation();
    m_startBtn->Enable(true);
    m_stopBtn->Enable(false);
}

void SchedulingPanel::OnResetSimulation(wxCommandEvent &event)
{
    m_ganttChart->ResetChart();
    m_startBtn->Enable(!m_processes.empty());
    m_stopBtn->Enable(false);
}

void SchedulingPanel::OnAlgorithmCheck(wxCommandEvent &event)
{
    // 1) Si selecciona FIFO, deseleccionar los demás (opcional)
    if (event.GetId() == 1010 && m_fifoCheck->GetValue())
    {
        // Desmarcar SJF, SRT, RR, Priority (si no quieres que se mezclen)
        m_sjfCheck->SetValue(false);
        m_srtCheck->SetValue(false);
        m_rrCheck->SetValue(false);
        m_priorityCheck->SetValue(false);
    }
    // … Similar para los demás checkboxes, si solo permites uno a la vez …

    // 2) Habilitar o deshabilitar el botón “Iniciar Simulación”
    bool anySelected = m_fifoCheck->GetValue() || m_sjfCheck->GetValue() || m_srtCheck->GetValue() || m_rrCheck->GetValue() || m_priorityCheck->GetValue();
    m_startBtn->Enable(anySelected && !m_processes.empty());
}

void SchedulingPanel::OnQuantumChange(wxSpinEvent &event)
{
    // Actualizar quantum para Round Robin
}

void SchedulingPanel::LoadProcessesFromFile(const wxString &filename)
{
    m_processListCtrl->DeleteAllItems();
    m_processes.clear();

    std::ifstream file(filename.ToStdString());
    if (!file.is_open())
    {
        wxMessageBox("No se pudo abrir el archivo de procesos.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    std::string line;
    // Paleta de colores para cada proceso
    std::vector<wxColour> colors = {
        wxColour(255, 0, 0),   // Rojo
        wxColour(0, 0, 255),   // Azul
        wxColour(0, 255, 0),   // Verde
        wxColour(0, 255, 255), // Cian
        wxColour(255, 255, 0), // Amarillo
        wxColour(255, 0, 255)  // Magenta
    };
    int colorIndex = 0;

    while (std::getline(file, line))
    {
        if (line.empty())
            continue; // saltar líneas en blanco

        std::stringstream ss(line);
        std::string pid_str, bt_str, at_str, prio_str;

        // Leer cada campo con getline y coma como separador
        std::getline(ss, pid_str, ',');
        std::getline(ss, bt_str, ',');
        std::getline(ss, at_str, ',');
        std::getline(ss, prio_str, ',');

        // Eliminar espacios al inicio y al final de cada string
        auto trim = [&](std::string &s)
        {
            // trim left
            s.erase(0, s.find_first_not_of(" \t\r\n"));
            // trim right
            s.erase(s.find_last_not_of(" \t\r\n") + 1);
        };
        trim(pid_str);
        trim(bt_str);
        trim(at_str);
        trim(prio_str);

        // Convertir a enteros
        int bt = std::stoi(bt_str);
        int at = std::stoi(at_str);
        int prio = std::stoi(prio_str);

        // Elegir color cíclicamente
        wxColour color = colors[colorIndex++ % colors.size()];
        Process p;
        p.pid = wxString(pid_str);
        p.burstTime = bt;
        p.arrivalTime = at;
        p.priority = prio;
        p.color = color;
        p.startTime = 0;
        p.finishTime = 0;
        p.waitingTime = 0;

        m_processes.push_back(p);
    }

    // Actualizar el wxListCtrl de la vista
    for (size_t i = 0; i < m_processes.size(); ++i)
    {
        long index = m_processListCtrl->InsertItem(i, m_processes[i].pid);
        m_processListCtrl->SetItem(index, 1, wxString::Format("%d", m_processes[i].burstTime));
        m_processListCtrl->SetItem(index, 2, wxString::Format("%d", m_processes[i].arrivalTime));
        m_processListCtrl->SetItem(index, 3, wxString::Format("%d", m_processes[i].priority));
    }

    // Activar botón “Iniciar” si ya hay procesos y al menos un algoritmo está seleccionado
    bool anyAlgSelected = m_fifoCheck->GetValue() ||
                          m_sjfCheck->GetValue() ||
                          m_srtCheck->GetValue() ||
                          m_rrCheck->GetValue() ||
                          m_priorityCheck->GetValue();
    m_startBtn->Enable(anyAlgSelected && !m_processes.empty());

    // Pasarle los procesos al Gantt (para la vista gráfica)
    m_ganttChart->SetProcesses(m_processes);
    UpdateMetrics();
}

void SchedulingPanel::ScheduleFIFO()
{
    // 1) Si no hay procesos cargados, salimos.
    if (m_processes.empty())
    {
        wxMessageBox("No hay procesos cargados.", "Atención", wxICON_INFORMATION);
        return;
    }

    // 2) Crear un vector de punteros a Process para actualizar directamente m_processes.
    std::vector<Process *> ptrs;
    ptrs.reserve(m_processes.size());
    for (auto &p : m_processes)
    {
        ptrs.push_back(&p);
    }

    // 3) Ordenar los punteros por arrivalTime (FIFO). En caso de empate, desempatar por PID.
    std::sort(ptrs.begin(), ptrs.end(), [](const Process *a, const Process *b)
              {
 if (a->arrivalTime != b->arrivalTime) {
 return a->arrivalTime < b->arrivalTime;
 }
 return a->pid < b->pid; });

    // 4) Recorrer en orden y calcular startTime/finishTime/waitingTime directamente sobre m_processes
    int currentCycle = 0;
    for (auto *p : ptrs)
    {
        int inicio = std::max(currentCycle, p->arrivalTime);
        p->startTime = inicio;
        p->finishTime = inicio + p->burstTime;
        p->waitingTime = p->startTime - p->arrivalTime;
        currentCycle = p->finishTime;
    }

    // 5) Actualizar métricas:
    double sumaWT = 0.0;
    double sumaTAT = 0.0;
    int ultimoFin = 0;

    for (const auto &p : m_processes)
    {
        sumaWT += p.waitingTime;
        sumaTAT += (p.finishTime - p.arrivalTime);
        ultimoFin = std::max(ultimoFin, p.finishTime);
    }

    double avgWT = sumaWT / m_processes.size();
    double avgTAT = sumaTAT / m_processes.size();
    double throughput = double(m_processes.size()) / double(ultimoFin);

    m_metricsGrid->SetCellValue(0, 1, wxString::Format("%.2f", avgWT));
    m_metricsGrid->SetCellValue(1, 1, wxString::Format("%.2f", avgTAT));
    m_metricsGrid->SetCellValue(2, 1, wxString::Format("%.2f", throughput));

    // 6) Pasar estos procesos al Gantt y reiniciar el diagrama para que empiece a pintar desde cero
    m_ganttChart->SetProcesses(m_processes);
    m_ganttChart->ResetChart();
}

void SchedulingPanel::ScheduleSJF()
{
    // 1) Si no hay procesos cargados, salimos
    if (m_processes.empty())
        return;

    // 2) Creamos un vector local "ordenados" con copia de todos los procesos
    // y los ordenamos inicialmente por arrivalTime (para saber quién llega primero).
    std::vector<Process> ordenados = m_processes;
    std::sort(ordenados.begin(), ordenados.end(),
              [](const Process &a, const Process &b)
              {
                  return a.arrivalTime < b.arrivalTime;
              });

    // 3) Preparar la "ready queue" donde iremos metiendo punteros a procesos ya llegados
    std::vector<Process *> readyQueue;
    int currentCycle = 0;
    size_t idx = 0; // índice para ir introduciendo elementos de "ordenados"

    // 4) Mientras queden procesos sin ejecutar (o listos en readyQueue)...
    while (idx < ordenados.size() || !readyQueue.empty())
    {
        // 4a) Mover a readyQueue todos los procesos cuya arrivalTime <= currentCycle
        while (idx < ordenados.size() && ordenados[idx].arrivalTime <= currentCycle)
        {
            readyQueue.push_back(&ordenados[idx]);
            idx++;
        }

        // 4b) Si no hay procesos en readyQueue, avanzamos currentCycle al próximo arrivalTime
        if (readyQueue.empty())
        {
            currentCycle = ordenados[idx].arrivalTime;
            continue;
        }

        // 4c) De la readyQueue, elegimos el que tenga el burstTime más pequeño
        auto it = std::min_element(readyQueue.begin(), readyQueue.end(),
                                   [](Process *a, Process *b)
                                   {
                                       return a->burstTime < b->burstTime;
                                   });
        Process *elegido = *it;
        readyQueue.erase(it); // lo sacamos de la cola

        // 4d) Calculamos startTime, finishTime y waitingTime del proceso "elegido"
        int inicio = std::max(currentCycle, elegido->arrivalTime);
        elegido->startTime = inicio;
        elegido->finishTime = inicio + elegido->burstTime;
        elegido->waitingTime = elegido->startTime - elegido->arrivalTime;

        // 4e) Actualizamos el reloj al finishTime de este proceso
        currentCycle = elegido->finishTime;
    }

    // 5) Copiar startTime/finishTime/waitingTime de "ordenados" a "m_processes" (coincidiendo por pid)
    for (const auto &p_done : ordenados)
    {
        for (auto &orig : m_processes)
        {
            if (orig.pid == p_done.pid)
            {
                orig.startTime = p_done.startTime;
                orig.finishTime = p_done.finishTime;
                orig.waitingTime = p_done.waitingTime;
                break;
            }
        }
    }

    // 6) Calcular métricas de eficiencia (Avg Waiting Time, Avg Turnaround Time, Throughput)
    double sumaWT = 0.0, sumaTAT = 0.0;
    int ultimoFin = 0;
    for (const auto &proc : m_processes)
    {
        sumaWT += proc.waitingTime;
        sumaTAT += (proc.finishTime - proc.arrivalTime);
        if (proc.finishTime > ultimoFin)
            ultimoFin = proc.finishTime;
    }
    double avgWT = sumaWT / m_processes.size();
    double avgTAT = sumaTAT / m_processes.size();
    double throughput = double(m_processes.size()) / double(ultimoFin);

    // 7) Actualizar la grilla de métricas
    m_metricsGrid->SetCellValue(0, 1, wxString::Format("%.2f", avgWT));
    m_metricsGrid->SetCellValue(1, 1, wxString::Format("%.2f", avgTAT));
    m_metricsGrid->SetCellValue(2, 1, wxString::Format("%.2f", throughput));

    // 8) Pasamos estos procesos (con start/finish/waiting actualizados) al Gantt y reiniciamos el chart
    m_ganttChart->SetProcesses(m_processes);
    m_ganttChart->ResetChart();
}

void SchedulingPanel::ScheduleSRT()
{
    if (m_processes.empty())
    {
        wxMessageBox("No hay procesos cargados.", "Atención", wxICON_INFORMATION);
        return;
    }

    struct ExecState
    {
        Process *proc;
        int remainingTime;
    };

    int n = m_processes.size();
    // 1) Inicializar cada proceso
    for (auto &p : m_processes)
    {
        p.startTime = -1;
        p.finishTime = -1;
        p.waitingTime = 0;
        p.segments.clear();
    }

    // 2) Ordenar punteros a procesos por arrivalTime para insertar en readyQueue
    std::vector<Process *> arrivals;
    arrivals.reserve(n);
    for (auto &p : m_processes)
    {
        arrivals.push_back(&p);
    }
    std::sort(arrivals.begin(), arrivals.end(),
              [](Process *a, Process *b)
              {
                  return a->arrivalTime < b->arrivalTime;
              });

    int idx = 0; // índice en ‘arrivals’
    int completed = 0;
    int currentCycle = 0;
    std::vector<ExecState> readyQueue;
    readyQueue.reserve(n);

    // 3) Ciclo principal
    while (completed < n)
    {
        // 3a) Agregar todos los procesos cuyo arrivalTime == currentCycle
        while (idx < n && arrivals[idx]->arrivalTime == currentCycle)
        {
            readyQueue.push_back({arrivals[idx], arrivals[idx]->burstTime});
            idx++;
        }

        // 3b) Seleccionar el proceso con menor remainingTime
        if (!readyQueue.empty())
        {
            auto itMin = std::min_element(
                readyQueue.begin(), readyQueue.end(),
                [](const ExecState &a, const ExecState &b)
                {
                    return a.remainingTime < b.remainingTime;
                });
            ExecState &exec = *itMin;
            Process *p = exec.proc;

            // 3c) Registrar segmento en p->segments
            if (p->segments.empty() ||
                p->segments.back().first + p->segments.back().second != currentCycle)
            {
                p->segments.push_back({currentCycle, 1});
            }
            else
            {
                p->segments.back().second += 1;
            }

            // 3d) Reducir tiempo restante
            exec.remainingTime--;
            if (exec.remainingTime == 0)
            {
                p->finishTime = currentCycle + 1;
                completed++;
                readyQueue.erase(itMin);
            }
        }

        currentCycle++;
    }

    // 4) Calcular startTime y waitingTime
    for (auto &p : m_processes)
    {
        if (!p.segments.empty())
        {
            p.startTime = p.segments.front().first;
            p.waitingTime = p.finishTime - p.arrivalTime - p.burstTime;
        }
    }

    // 5) Calcular métricas
    double totalWT = 0.0, totalTAT = 0.0;
    int ultimoFin = 0;
    for (const auto &p : m_processes)
    {
        totalWT += p.waitingTime;
        totalTAT += (p.finishTime - p.arrivalTime);
        ultimoFin = std::max(ultimoFin, p.finishTime);
    }
    double avgWT = totalWT / n;
    double avgTAT = totalTAT / n;
    double throughput = double(n) / double(ultimoFin);

    m_metricsGrid->SetCellValue(0, 1, wxString::Format("%.2f", avgWT));
    m_metricsGrid->SetCellValue(1, 1, wxString::Format("%.2f", avgTAT));
    m_metricsGrid->SetCellValue(2, 1, wxString::Format("%.2f", throughput));

    // 6) Enviar a Gantt y reiniciar
    m_ganttChart->SetProcesses(m_processes);
    m_ganttChart->ResetChart();
}

void SchedulingPanel::ScheduleRR()
{
    if (m_processes.empty())
        return;

    int quantum = m_quantumSpin->GetValue();
    std::vector<Process> procesos = m_processes;
    int n = procesos.size();

    for (auto &p : procesos)
    {
        p.startTime = -1;
        p.finishTime = -1;
        p.waitingTime = 0;
        p.segments.clear();
    }

    std::queue<Process *> readyQueue;
    int currentCycle = 0;
    int completed = 0;

    std::vector<int> remainingBT(n);
    for (int i = 0; i < n; ++i)
    {
        remainingBT[i] = procesos[i].burstTime;
    }

    std::vector<bool> inQueue(n, false);

    while (completed < n)
    {
        // Agregar procesos que llegan este ciclo
        for (int i = 0; i < n; ++i)
        {
            if (procesos[i].arrivalTime == currentCycle && !inQueue[i])
            {
                readyQueue.push(&procesos[i]);
                inQueue[i] = true;
            }
        }

        if (readyQueue.empty())
        {
            currentCycle++;
            continue;
        }

        Process *p = readyQueue.front();
        readyQueue.pop();

        int index = std::distance(procesos.begin(), std::find_if(procesos.begin(), procesos.end(),
                                                                 [&](const Process &proc)
                                                                 { return proc.pid == p->pid; }));
        int execTime = std::min(quantum, remainingBT[index]);

        // Añadir segmento RR
        p->segments.push_back({currentCycle, execTime});

        remainingBT[index] -= execTime;
        currentCycle += execTime;

        // Añadir procesos que llegaron mientras este ejecutaba
        for (int i = 0; i < n; ++i)
        {
            if (procesos[i].arrivalTime > currentCycle - execTime &&
                procesos[i].arrivalTime <= currentCycle && !inQueue[i])
            {
                readyQueue.push(&procesos[i]);
                inQueue[i] = true;
            }
        }

        if (remainingBT[index] > 0)
        {
            readyQueue.push(p); // volver a encolar
        }
        else
        {
            p->finishTime = currentCycle;
            completed++;
        }
    }

    for (auto &p : procesos)
    {
        p.startTime = p.segments.front().first;
        int execution = 0;
        for (const auto &seg : p.segments)
        {
            execution += seg.second;
        }
        p.waitingTime = p.finishTime - p.arrivalTime - execution;
    }

    // Copiar a procesos reales
    for (auto &p_new : procesos)
    {
        for (auto &p_old : m_processes)
        {
            if (p_new.pid == p_old.pid)
            {
                p_old.startTime = p_new.startTime;
                p_old.finishTime = p_new.finishTime;
                p_old.waitingTime = p_new.waitingTime;
                p_old.segments = p_new.segments;
            }
        }
    }

    // Métricas
    double totalWT = 0, totalTAT = 0;
    int ultimoFin = 0;
    for (const auto &p : m_processes)
    {
        totalWT += p.waitingTime;
        totalTAT += (p.finishTime - p.arrivalTime);
        ultimoFin = std::max(ultimoFin, p.finishTime);
    }

    m_metricsGrid->SetCellValue(0, 1, wxString::Format("%.2f", totalWT / n));
    m_metricsGrid->SetCellValue(1, 1, wxString::Format("%.2f", totalTAT / n));
    m_metricsGrid->SetCellValue(2, 1, wxString::Format("%.2f", double(n) / ultimoFin));

    // Visualización
    m_ganttChart->SetProcesses(m_processes);
    m_ganttChart->ResetChart();
}

void SchedulingPanel::SchedulePriority()
{
    if (m_processes.empty())
        return;

    std::vector<Process> procesos = m_processes;
    int n = procesos.size();

    for (auto &p : procesos)
    {
        p.startTime = -1;
        p.finishTime = -1;
        p.waitingTime = 0;
        p.segments.clear();
    }

    std::vector<Process *> readyQueue;
    int currentCycle = 0;
    size_t idx = 0;

    std::sort(procesos.begin(), procesos.end(), [](const Process &a, const Process &b)
              { return a.arrivalTime < b.arrivalTime; });

    while (idx < procesos.size() || !readyQueue.empty())
    {
        while (idx < procesos.size() && procesos[idx].arrivalTime <= currentCycle)
        {
            readyQueue.push_back(&procesos[idx]);
            idx++;
        }

        if (readyQueue.empty())
        {
            currentCycle = procesos[idx].arrivalTime;
            continue;
        }

        auto it = std::min_element(readyQueue.begin(), readyQueue.end(),
                                   [](Process *a, Process *b)
                                   {
                                       return a->priority < b->priority;
                                   });

        Process *elegido = *it;
        readyQueue.erase(it);

        int inicio = std::max(currentCycle, elegido->arrivalTime);
        elegido->startTime = inicio;
        elegido->finishTime = inicio + elegido->burstTime;
        elegido->waitingTime = elegido->startTime - elegido->arrivalTime;
        elegido->segments.push_back({inicio, elegido->burstTime});
        currentCycle = elegido->finishTime;
    }

    for (const auto &p_src : procesos)
    {
        for (auto &p_dst : m_processes)
        {
            if (p_dst.pid == p_src.pid)
            {
                p_dst.startTime = p_src.startTime;
                p_dst.finishTime = p_src.finishTime;
                p_dst.waitingTime = p_src.waitingTime;
                p_dst.segments = p_src.segments;
            }
        }
    }

    double totalWT = 0, totalTAT = 0;
    int ultimoFin = 0;
    for (const auto &p : m_processes)
    {
        totalWT += p.waitingTime;
        totalTAT += (p.finishTime - p.arrivalTime);
        ultimoFin = std::max(ultimoFin, p.finishTime);
    }

    m_metricsGrid->SetCellValue(0, 1, wxString::Format("%.2f", totalWT / n));
    m_metricsGrid->SetCellValue(1, 1, wxString::Format("%.2f", totalTAT / n));
    m_metricsGrid->SetCellValue(2, 1, wxString::Format("%.2f", double(n) / ultimoFin));

    m_ganttChart->SetProcesses(m_processes);
    m_ganttChart->ResetChart();
}

void SchedulingPanel::UpdateMetrics()
{
    // Calcular y mostrar metricas
    m_metricsGrid->SetCellValue(0, 1, "5.2");
    m_metricsGrid->SetCellValue(1, 1, "8.7");
    m_metricsGrid->SetCellValue(2, 1, "0.8");
}

// Implementaciones similares para SynchronizationPanel
void SynchronizationPanel::OnLoadProcesses(wxCommandEvent &event)
{
    wxFileDialog dialog(this, "Seleccionar archivo de procesos", "", "",
                        "Archivos de texto (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (dialog.ShowModal() == wxID_OK)
    {
        LoadProcessesFromFile(dialog.GetPath());
    }
}

void SynchronizationPanel::OnLoadResources(wxCommandEvent &event)
{
    wxFileDialog dialog(this, "Seleccionar archivo de recursos", "", "",
                        "Archivos de texto (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (dialog.ShowModal() == wxID_OK)
    {
        LoadResourcesFromFile(dialog.GetPath());
    }
}

void SynchronizationPanel::OnLoadActions(wxCommandEvent &event)
{
    wxFileDialog dialog(this, "Seleccionar archivo de acciones", "", "",
                        "Archivos de texto (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (dialog.ShowModal() == wxID_OK)
    {
        LoadActionsFromFile(dialog.GetPath());
    }
}

void SynchronizationPanel::OnStartSimulation(wxCommandEvent &event)
{
    m_timelineChart->StartSimulation();
    m_startBtn->Enable(false);
    m_stopBtn->Enable(true);
}

void SynchronizationPanel::OnStopSimulation(wxCommandEvent &event)
{
    m_timelineChart->StopSimulation();
    m_startBtn->Enable(true);
    m_stopBtn->Enable(false);
}

void SynchronizationPanel::OnResetSimulation(wxCommandEvent &event)
{
    m_timelineChart->ResetChart();
    m_startBtn->Enable(!m_processes.empty() && !m_resources.empty() && !m_actions.empty());
    m_stopBtn->Enable(false);
}

void SynchronizationPanel::OnSyncModeChange(wxCommandEvent &event)
{
    // Actualizar configuracion segun el modo de sincronizacion seleccionado
    wxString mode = m_syncModeChoice->GetStringSelection();
    // Corregir acceso al StatusBar
    MainFrame *mainFrame = dynamic_cast<MainFrame *>(GetParent()->GetParent());
    if (mainFrame)
    {
        mainFrame->SetStatusText("Modo: " + mode, 1);
    }
}

void SynchronizationPanel::LoadProcessesFromFile(const wxString &filename)
{
    // Limpiar la lista anterior y el vector m_processes
    m_processListCtrl->DeleteAllItems();
    m_processes.clear();

    std::ifstream file(filename.ToStdString());
    if (!file.is_open())
    {
        wxMessageBox("No se pudo abrir el archivo de procesos.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    std::string line;
    // Paleta de colores para cada proceso
    std::vector<wxColour> colors = {
        wxColour(255, 0, 0),   // Rojo
        wxColour(0, 0, 255),   // Azul
        wxColour(0, 255, 0),   // Verde
        wxColour(0, 255, 255), // Cian
        wxColour(255, 255, 0), // Amarillo
        wxColour(255, 0, 255)  // Magenta
    };
    int colorIndex = 0;

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::stringstream ss(line);
        std::string pid_str, bt_str, at_str, prio_str;

        // Leemos cada campo hasta la coma
        std::getline(ss, pid_str, ',');
        std::getline(ss, bt_str, ',');
        std::getline(ss, at_str, ',');
        std::getline(ss, prio_str, ',');

        // Función corta para eliminar espacios al inicio y fin
        auto trim = [&](std::string &s)
        {
            s.erase(0, s.find_first_not_of(" \t\r\n"));
            s.erase(s.find_last_not_of(" \t\r\n") + 1);
        };
        trim(pid_str);
        trim(bt_str);
        trim(at_str);
        trim(prio_str);

        int bt = std::stoi(bt_str);
        int at = std::stoi(at_str);
        int prio = std::stoi(prio_str);

        wxColour color = colors[colorIndex++ % colors.size()];
        Process p;
        p.pid = wxString(pid_str);
        p.burstTime = bt;
        p.arrivalTime = at;
        p.priority = prio;
        p.color = color;
        p.startTime = 0;
        p.finishTime = 0;
        p.waitingTime = 0;
        m_processes.push_back(p);
    }

    // Ahora actualizamos el ListCtrl en el panel de sincronización
    for (size_t i = 0; i < m_processes.size(); ++i)
    {
        long idx = m_processListCtrl->InsertItem(i, m_processes[i].pid);
        m_processListCtrl->SetItem(idx, 1, wxString::Format("%d", m_processes[i].burstTime));
        m_processListCtrl->SetItem(idx, 2, wxString::Format("%d", m_processes[i].arrivalTime));
        m_processListCtrl->SetItem(idx, 3, wxString::Format("%d", m_processes[i].priority));
    }

    CheckEnableStart();
}

void SynchronizationPanel::LoadResourcesFromFile(const wxString &filename)
{
    // Limpiar la lista y el vector de recursos anteriores
    m_resourceListCtrl->DeleteAllItems();
    m_resources.clear();

    std::ifstream file(filename.ToStdString());
    if (!file.is_open())
    {
        wxMessageBox("No se pudo abrir el archivo de recursos.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue; // saltar líneas en blanco

        std::stringstream ss(line);
        std::string name_str, counter_str;

        // Leer cada campo hasta la coma
        std::getline(ss, name_str, ',');
        std::getline(ss, counter_str, ',');

        // Eliminar espacios al inicio y al final de cada string
        auto trim = [&](std::string &s)
        {
            s.erase(0, s.find_first_not_of(" \t\r\n")); // trim left
            s.erase(s.find_last_not_of(" \t\r\n") + 1); // trim right
        };
        trim(name_str);
        trim(counter_str);

        // Convertir counter a entero seguro
        int counter = std::stoi(counter_str);

        Resource r = {wxString(name_str), counter};
        m_resources.push_back(r);
    }

    // Llenar el wxListCtrl con los recursos cargados
    for (size_t i = 0; i < m_resources.size(); ++i)
    {
        long idx = m_resourceListCtrl->InsertItem(i, m_resources[i].name);
        m_resourceListCtrl->SetItem(idx, 1,
                                    wxString::Format("%d", m_resources[i].counter));
    }

    // Verificar si ya se pueden habilitar los botones de “Iniciar Simulación”
    CheckEnableStart();
}

void SynchronizationPanel::LoadActionsFromFile(const wxString &filename)
{
    // Implementar carga de acciones desde archivo
    // Formato: <PID>, <ACCION>, <RECURSO>, <CICLO>
    m_actionListCtrl->DeleteAllItems();
    m_actions.clear();

    std::ifstream file(filename.ToStdString());
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string pid, accion, recurso;
        int ciclo;
        char coma;

        std::getline(ss, pid, ',');
        std::getline(ss, accion, ',');
        std::getline(ss, recurso, ',');
        ss >> ciclo;

        // Limpiar espacios
        pid.erase(0, pid.find_first_not_of(" \t\r\n"));
        pid.erase(pid.find_last_not_of(" \t\r\n") + 1);
        accion.erase(0, accion.find_first_not_of(" \t\r\n"));
        accion.erase(accion.find_last_not_of(" \t\r\n") + 1);
        recurso.erase(0, recurso.find_first_not_of(" \t\r\n"));
        recurso.erase(recurso.find_last_not_of(" \t\r\n") + 1);

        Action a = {wxString(pid), wxString(accion), wxString(recurso), ciclo};
        m_actions.push_back(a);
    }

    // Actualizar lista visual
    for (size_t i = 0; i < m_actions.size(); ++i)
    {
        long index = m_actionListCtrl->InsertItem(i, m_actions[i].pid);
        m_actionListCtrl->SetItem(index, 1, m_actions[i].action);
        m_actionListCtrl->SetItem(index, 2, m_actions[i].resource);
        m_actionListCtrl->SetItem(index, 3, wxString::Format("%d", m_actions[i].cycle));
    }

    CheckEnableStart();

    // Actualizar datos en el timeline
    m_timelineChart->SetData(m_processes, m_resources, m_actions);
}

void SynchronizationPanel::CheckEnableStart()
{
    bool canStart = !m_processes.empty() && !m_resources.empty() && !m_actions.empty();
    m_startBtn->Enable(canStart);
}

// Implementaciones de los metodos de dibujo para GanttChart
void GanttChart::OnPaint(wxPaintEvent &event)
{
    wxPaintDC dc(this);
    DoPrepareDC(dc);

    // Limpiar fondo
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();

    if (m_processes.empty())
    {
        dc.DrawText("No hay procesos cargados", 10, 10);
        return;
    }

    DrawTimeAxis(dc);
    DrawProcessBlocks(dc);

    // Mostrar ciclo actual
    dc.SetTextForeground(*wxBLACK);
    dc.DrawText(wxString::Format("Ciclo actual: %d", m_currentCycle), 10, 10);
}

void GanttChart::DrawTimeAxis(wxPaintDC &dc)
{
    dc.SetPen(*wxBLACK_PEN);
    int baseY = 60;
    dc.DrawLine(50, baseY, 800, baseY);

    for (int i = 0; i <= 20; ++i)
    {
        int x = 50 + i * 30; // 30px por ciclo
        dc.DrawLine(x, baseY - 5, x, baseY + 5);
        dc.DrawText(wxString::Format("%d", i), x - 5, baseY + 10);
    }
}

void GanttChart::DrawProcessBlocks(wxPaintDC &dc)
{
    const int baseY = 80;       // Y de inicio
    const int rowHeight = 30;   // Altura entre filas
    const int blockHeight = 25; // Alto del bloque de proceso
    const int pxPerCycle = 30;  // Escala horizontal
    const int marginX = 50;     // Margen izquierdo

    for (size_t i = 0; i < m_processes.size(); ++i)
    {
        const Process &proc = m_processes[i];
        int y = baseY + static_cast<int>(i) * rowHeight;

        // Si hay segmentos definidos (caso SRT, RR, FIFO adaptado...)
        if (!proc.segments.empty())
        {
            for (const auto &seg : proc.segments)
            {
                int start = seg.first;
                int length = seg.second;
                int x = marginX + start * pxPerCycle;
                int w = length * pxPerCycle;

                dc.SetBrush(wxBrush(proc.color));
                dc.SetPen(wxPen(proc.color.ChangeLightness(80), 2));
                dc.DrawRectangle(x, y, w, blockHeight);

                // Etiquetar PID dentro del bloque
                dc.SetTextForeground(*wxWHITE);
                dc.DrawText(proc.pid, x + 5, y + 5);
            }
        }
        // Si no hay segmentos, usar startTime y burstTime como antes (para compatibilidad)
        else if (proc.burstTime > 0)
        {
            int x = marginX + proc.startTime * pxPerCycle;
            int w = proc.burstTime * pxPerCycle;

            if (m_isRunning && m_currentCycle >= proc.startTime)
            {
                int progreso = std::min(proc.burstTime, m_currentCycle - proc.startTime);
                int anchoProgreso = progreso * pxPerCycle;

                dc.SetBrush(wxBrush(proc.color));
                dc.SetPen(wxPen(proc.color.ChangeLightness(80), 2));
                dc.DrawRectangle(x, y, anchoProgreso, blockHeight);

                if (anchoProgreso < w)
                {
                    dc.SetBrush(wxBrush(wxColour(200, 200, 200)));
                    dc.SetPen(wxPen(*wxLIGHT_GREY, 1));
                    dc.DrawRectangle(x + anchoProgreso, y, w - anchoProgreso, blockHeight);
                }
            }
            else
            {
                dc.SetBrush(wxBrush(proc.color));
                dc.SetPen(wxPen(proc.color.ChangeLightness(80), 2));
                dc.DrawRectangle(x, y, w, blockHeight);
            }

            dc.SetTextForeground(*wxWHITE);
            dc.DrawText(proc.pid, x + 5, y + 5);
        }
    }
}

void GanttChart::OnTimer(wxTimerEvent &event)
{
    if (m_isRunning)
    {
        m_currentCycle++;

        // Actualizar scroll automaticamente
        int x, y;
        GetViewStart(&x, &y);
        if (m_currentCycle * 30 > GetSize().GetWidth() + x * 20)
        {
            Scroll(x + 5, y);
        }

        Refresh();

        // Detener cuando todos los procesos hayan terminado
        if (m_currentCycle > 50)
        { // Condicion de ejemplo
            StopSimulation();
        }
    }
}

void GanttChart::StartSimulation()
{
    m_isRunning = true;
    m_timer->Start(500); // 500ms por ciclo
}

void GanttChart::StopSimulation()
{
    m_isRunning = false;
    m_timer->Stop();
}

void GanttChart::ResetChart()
{
    m_currentCycle = 0;
    m_isRunning = false;
    m_timer->Stop();
    Scroll(0, 0);
    Refresh();
}

void GanttChart::SetProcesses(const std::vector<Process> &processes)
{
    m_processes = processes;
    Refresh();
}

// Implementaciones para TimelineChart
void TimelineChart::OnPaint(wxPaintEvent &event)
{
    wxPaintDC dc(this);
    DoPrepareDC(dc);

    // Limpiar fondo
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();

    if (m_processes.empty() || m_resources.empty() || m_actions.empty())
    {
        dc.DrawText("Cargar procesos, recursos y acciones", 10, 10);
        return;
    }

    DrawTimeline(dc);

    // Mostrar ciclo actual
    dc.SetTextForeground(*wxBLACK);
    dc.DrawText(wxString::Format("Ciclo actual: %d", m_currentCycle), 10, 10);
}

void TimelineChart::DrawTimeline(wxPaintDC &dc)
{
    dc.SetPen(*wxBLACK_PEN);

    // Línea base del tiempo
    int baseY = 60;
    dc.DrawLine(50, baseY, 800, baseY);

    // Marcas de tiempo
    for (int i = 0; i <= 20; ++i)
    {
        int x = 50 + i * 30;
        dc.DrawLine(x, baseY - 5, x, baseY + 5);
        dc.DrawText(wxString::Format("%d", i), x - 5, baseY + 10);
    }

    // Dibujar acciones por proceso
    int rowHeight = 30;
    for (size_t i = 0; i < m_processes.size(); ++i)
    {
        const Process &process = m_processes[i];
        int y = baseY + 20 + i * rowHeight;

        dc.DrawText(process.pid, 10, y);

        for (const Action &action : m_actions)
        {
            if (action.pid == process.pid && action.cycle <= m_currentCycle)
            {
                auto key = std::make_tuple(action.pid, action.resource, action.cycle);
                int x = 50 + action.cycle * 30;
                wxColour color;

                // --- lógica de sincronización (mutex/semaforo) ---
                bool esMutex = (m_syncMode == "Mutex Locks");

                if (m_actionsDone.count(key))
                {
                    // Ya ejecutada, mostrar normalmente
                    color = (action.action == "READ") ? wxColour(100, 200, 100)
                                                      : wxColour(200, 100, 100);
                }
                else if ((esMutex && m_resourceStates[action.resource] == 1) ||
                         (!esMutex && m_resourceStates[action.resource] > 0))
                {
                    // ACCESSED
                    color = (action.action == "READ") ? wxColour(100, 200, 100)
                                                      : wxColour(200, 100, 100);

                    m_resourceStates[action.resource]--;
                    m_pendingReleases[action.resource].push(m_currentCycle + 1); // liberar en siguiente ciclo
                    m_actionsDone.insert(key);
                }
                else
                {
                    // WAITING
                    color = wxColour(200, 200, 100);
                }

                dc.SetBrush(wxBrush(color));
                dc.DrawRectangle(x - 10, y, 20, 20);
                dc.SetTextForeground(*wxBLACK);
                dc.DrawText(action.action.Left(1), x - 5, y + 2);
            }
        }
    }

    // Leyenda
    int legendY = baseY + 20 + m_processes.size() * rowHeight + 20;
    dc.DrawText("Leyenda:", 10, legendY);

    dc.SetBrush(wxBrush(wxColour(100, 200, 100)));
    dc.DrawRectangle(80, legendY, 15, 15);
    dc.DrawText("READ", 100, legendY);

    dc.SetBrush(wxBrush(wxColour(200, 100, 100)));
    dc.DrawRectangle(150, legendY, 15, 15);
    dc.DrawText("WRITE", 170, legendY);

    dc.SetBrush(wxBrush(wxColour(200, 200, 100)));
    dc.DrawRectangle(220, legendY, 15, 15);
    dc.DrawText("WAITING", 240, legendY);
}

void TimelineChart::OnTimer(wxTimerEvent &event)
{
    if (m_isRunning)
    {
        m_currentCycle++;

        for (auto &[recurso, ciclos] : m_pendingReleases)
        {
            while (!ciclos.empty() && ciclos.front() == m_currentCycle)
            {
                m_resourceStates[recurso]++;
                ciclos.pop();
            }
        }

        // Scroll automático si se pasa del ancho visible
        int x, y;
        GetViewStart(&x, &y);
        if (m_currentCycle * 30 > GetSize().GetWidth() + x * 20)
        {
            Scroll(x + 5, y);
        }

        Refresh();

        // Detener cuando se hayan procesado todas las acciones
        int maxCycle = 0;
        for (const Action &action : m_actions)
        {
            maxCycle = std::max(maxCycle, action.cycle);
        }

        if (m_currentCycle > maxCycle + 5)
        {
            StopSimulation();
        }
    }
}

void TimelineChart::StartSimulation()
{
    m_isRunning = true;
    m_timer->Start(800); // 800ms por ciclo para sincronizacion
}

void TimelineChart::StopSimulation()
{
    m_isRunning = false;
    m_timer->Stop();
}

void TimelineChart::ResetChart()
{
    m_currentCycle = 0;
    m_isRunning = false;
    m_timer->Stop();
    Scroll(0, 0);
    m_actionStartCycle.clear();
    Refresh();
}

void TimelineChart::SetData(const std::vector<Process> &processes, const std::vector<Resource> &resources, const std::vector<Action> &actions)
{
    m_processes = processes;
    m_resources = resources;
    m_actions = actions;
    m_resourceStates.clear();
    m_actionsDone.clear();

    for (const auto &r : resources)
    {
        m_resourceStates[r.name] = r.counter;
    }

    Refresh();
}

// Punto de entrada de la aplicacion
wxIMPLEMENT_APP(OSSimulatorApp);