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

class MainFrame;
class GanttChart;
class TimelineChart;

// Estructuras de datos
struct Process {
    wxString pid;
    int burstTime;
    int arrivalTime;
    int priority;
    wxColour color;
};

struct Resource {
    wxString name;
    int counter;
};

struct Action {
    wxString pid;
    wxString action;
    wxString resource;
    int cycle;
};

// Ventana principal
class OSSimulatorApp : public wxApp {
public:
    bool OnInit() override;
};

// Panel para visualización diagrama de  Gantt
class GanttChart : public wxScrolledWindow {
public:
    GanttChart(wxWindow* parent);
    void StartSimulation();
    void StopSimulation();
    void ResetChart();
    void SetProcesses(const std::vector<Process>& processes);
    
private:
    void OnPaint(wxPaintEvent& event);
    void OnTimer(wxTimerEvent& event);
    void DrawTimeAxis(wxPaintDC& dc);
    void DrawProcessBlocks(wxPaintDC& dc);
    
    wxTimer* m_timer;
    std::vector<Process> m_processes;
    int m_currentCycle;
    bool m_isRunning;
    
    wxDECLARE_EVENT_TABLE();
};

// Panel para timeline de sincronización
class TimelineChart : public wxScrolledWindow {
public:
    TimelineChart(wxWindow* parent);
    void StartSimulation();
    void StopSimulation();
    void ResetChart();
    void SetData(const std::vector<Process>& processes, 
                 const std::vector<Resource>& resources,
                 const std::vector<Action>& actions);
    
private:
    void OnPaint(wxPaintEvent& event);
    void OnTimer(wxTimerEvent& event);
    void DrawTimeline(wxPaintDC& dc);
    
    wxTimer* m_timer;
    std::vector<Process> m_processes;
    std::vector<Resource> m_resources;
    std::vector<Action> m_actions;
    int m_currentCycle;
    bool m_isRunning;
    
    wxDECLARE_EVENT_TABLE();
};

// Panel de calendarización
class SchedulingPanel : public wxPanel {
public:
    SchedulingPanel(wxWindow* parent);
    
private:
    void OnLoadProcesses(wxCommandEvent& event);
    void OnStartSimulation(wxCommandEvent& event);
    void OnStopSimulation(wxCommandEvent& event);
    void OnResetSimulation(wxCommandEvent& event);
    void OnAlgorithmCheck(wxCommandEvent& event);
    void OnQuantumChange(wxSpinEvent& event);
    void LoadProcessesFromFile(const wxString& filename);
    void UpdateMetrics();
    
    wxCheckBox* m_fifoCheck;
    wxCheckBox* m_sjfCheck;
    wxCheckBox* m_srtCheck;
    wxCheckBox* m_rrCheck;
    wxCheckBox* m_priorityCheck;
    wxSpinCtrl* m_quantumSpin;
    wxButton* m_loadProcessesBtn;
    wxButton* m_startBtn;
    wxButton* m_stopBtn;
    wxButton* m_resetBtn;
    wxListCtrl* m_processListCtrl;
    wxGrid* m_metricsGrid;
    GanttChart* m_ganttChart;
    
    std::vector<Process> m_processes;
    
    wxDECLARE_EVENT_TABLE();
};

// Panel de sincronización
class SynchronizationPanel : public wxPanel {
public:
    SynchronizationPanel(wxWindow* parent);
    
private:
    void OnLoadProcesses(wxCommandEvent& event);
    void OnLoadResources(wxCommandEvent& event);
    void OnLoadActions(wxCommandEvent& event);
    void OnStartSimulation(wxCommandEvent& event);
    void OnStopSimulation(wxCommandEvent& event);
    void OnResetSimulation(wxCommandEvent& event);
    void OnSyncModeChange(wxCommandEvent& event);
    void LoadProcessesFromFile(const wxString& filename);
    void LoadResourcesFromFile(const wxString& filename);
    void LoadActionsFromFile(const wxString& filename);
    void CheckEnableStart();  
    
    wxChoice* m_syncModeChoice;
    wxButton* m_loadProcessesBtn;
    wxButton* m_loadResourcesBtn;
    wxButton* m_loadActionsBtn;
    wxButton* m_startBtn;
    wxButton* m_stopBtn;
    wxButton* m_resetBtn;
    wxListCtrl* m_processListCtrl;
    wxListCtrl* m_resourceListCtrl;
    wxListCtrl* m_actionListCtrl;
    TimelineChart* m_timelineChart;
    
    std::vector<Process> m_processes;
    std::vector<Resource> m_resources;
    std::vector<Action> m_actions;
    
    wxDECLARE_EVENT_TABLE();
};

// Frame principal
class MainFrame : public wxFrame {
public:
    MainFrame();
    
private:
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnNotebookPageChanged(wxBookCtrlEvent& event);
    
    wxNotebook* m_notebook;
    SchedulingPanel* m_schedulingPanel;
    SynchronizationPanel* m_syncPanel;
    
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
    EVT_CHECKBOX(wxID_ANY, SchedulingPanel::OnAlgorithmCheck)
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

bool OSSimulatorApp::OnInit() {
    MainFrame* frame = new MainFrame();
    frame->Show(true);
    return true;
}

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "Simulador de Sistemas Operativos", 
                                 wxDefaultPosition, wxSize(1200, 800)) {
    // Menu bar
    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, "E&xit\tCtrl-Q", "Quit this program");
    
    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, "&About\tF1", "Show about dialog");
    
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");
    SetMenuBar(menuBar);
    
    // Status bar
    CreateStatusBar(2);
    SetStatusText("Listo para simular", 0);
    
    // Notebook principal
    m_notebook = new wxNotebook(this, wxID_ANY);
    
    // Crear paneles
    m_schedulingPanel = new SchedulingPanel(m_notebook);
    m_syncPanel = new SynchronizationPanel(m_notebook);
    
    m_notebook->AddPage(m_schedulingPanel, "A. Calendarización", true);
    m_notebook->AddPage(m_syncPanel, "B. Sincronización", false);
    
    // Layout principal
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(m_notebook, 1, wxEXPAND | wxALL, 5);
    SetSizer(mainSizer);
    
    Centre();
}

SchedulingPanel::SchedulingPanel(wxWindow* parent) : wxPanel(parent) {
    // Panel de control superior
    wxStaticBoxSizer* controlBox = new wxStaticBoxSizer(wxHORIZONTAL, this, "Configuración");
    
    // Algoritmos
    wxStaticBoxSizer* algBox = new wxStaticBoxSizer(wxVERTICAL, this, "Algoritmos");
    m_fifoCheck = new wxCheckBox(this, wxID_ANY, "First In First Out (FIFO)");
    m_sjfCheck = new wxCheckBox(this, wxID_ANY, "Shortest Job First (SJF)");
    m_srtCheck = new wxCheckBox(this, wxID_ANY, "Shortest Remaining Time (SRT)");
    m_rrCheck = new wxCheckBox(this, wxID_ANY, "Round Robin (RR)");
    m_priorityCheck = new wxCheckBox(this, wxID_ANY, "Priority");
    
    algBox->Add(m_fifoCheck, 0, wxALL, 2);
    algBox->Add(m_sjfCheck, 0, wxALL, 2);
    algBox->Add(m_srtCheck, 0, wxALL, 2);
    algBox->Add(m_rrCheck, 0, wxALL, 2);
    algBox->Add(m_priorityCheck, 0, wxALL, 2);
    
    // Quantum para Round Robin
    wxStaticBoxSizer* quantumBox = new wxStaticBoxSizer(wxVERTICAL, this, "Quantum (RR)");
    m_quantumSpin = new wxSpinCtrl(this, 1005, "3", wxDefaultPosition, wxDefaultSize, 
                                   wxSP_ARROW_KEYS, 1, 100, 3);
    quantumBox->Add(new wxStaticText(this, wxID_ANY, "Ciclos:"), 0, wxALL, 2);
    quantumBox->Add(m_quantumSpin, 0, wxALL, 2);
    
    // Botones de control
    wxStaticBoxSizer* btnBox = new wxStaticBoxSizer(wxVERTICAL, this, "Control");
    m_loadProcessesBtn = new wxButton(this, 1001, "Cargar Procesos");
    m_startBtn = new wxButton(this, 1002, "Iniciar Simulación");
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
    
    // Panel de información
    wxStaticBoxSizer* infoBox = new wxStaticBoxSizer(wxHORIZONTAL, this, "Información");
    
    // Lista de procesos
    wxStaticBoxSizer* processBox = new wxStaticBoxSizer(wxVERTICAL, this, "Procesos Cargados");
    m_processListCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(300, 150), 
                                       wxLC_REPORT | wxLC_SINGLE_SEL);
    m_processListCtrl->AppendColumn("PID", wxLIST_FORMAT_LEFT, 60);
    m_processListCtrl->AppendColumn("BT", wxLIST_FORMAT_RIGHT, 60);
    m_processListCtrl->AppendColumn("AT", wxLIST_FORMAT_RIGHT, 60);
    m_processListCtrl->AppendColumn("Priority", wxLIST_FORMAT_RIGHT, 80);
    processBox->Add(m_processListCtrl, 1, wxEXPAND | wxALL, 2);
    
    // Métricas
    wxStaticBoxSizer* metricsBox = new wxStaticBoxSizer(wxVERTICAL, this, "Métricas de Eficiencia");
    m_metricsGrid = new wxGrid(this, wxID_ANY, wxDefaultPosition, wxSize(300, 150));
    m_metricsGrid->CreateGrid(3, 2);
    m_metricsGrid->SetColLabelValue(0, "Métrica");
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
    
    // Gráfico Gantt
    wxStaticBoxSizer* ganttBox = new wxStaticBoxSizer(wxVERTICAL, this, "Diagrama de Gantt");
    m_ganttChart = new GanttChart(this);
    ganttBox->Add(m_ganttChart, 1, wxEXPAND | wxALL, 2);
    
    // Layout principal
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(controlBox, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(infoBox, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(ganttBox, 1, wxEXPAND | wxALL, 5);
    
    SetSizer(mainSizer);
}

SynchronizationPanel::SynchronizationPanel(wxWindow* parent) : wxPanel(parent) {
    // Panel de control superior
    wxStaticBoxSizer* controlBox = new wxStaticBoxSizer(wxHORIZONTAL, this, "Configuración");
    
    // Modo de sincronización
    wxStaticBoxSizer* syncBox = new wxStaticBoxSizer(wxVERTICAL, this, "Mecanismo");
    wxArrayString syncChoices;
    syncChoices.Add("Mutex Locks");
    syncChoices.Add("Semáforos");
    m_syncModeChoice = new wxChoice(this, 2007, wxDefaultPosition, wxDefaultSize, syncChoices);
    m_syncModeChoice->SetSelection(0);
    syncBox->Add(new wxStaticText(this, wxID_ANY, "Tipo:"), 0, wxALL, 2);
    syncBox->Add(m_syncModeChoice, 0, wxEXPAND | wxALL, 2);
    
    // Botones de carga
    wxStaticBoxSizer* loadBox = new wxStaticBoxSizer(wxVERTICAL, this, "Carga de Archivos");
    m_loadProcessesBtn = new wxButton(this, 2001, "Cargar Procesos");
    m_loadResourcesBtn = new wxButton(this, 2002, "Cargar Recursos");
    m_loadActionsBtn = new wxButton(this, 2003, "Cargar Acciones");
    loadBox->Add(m_loadProcessesBtn, 0, wxEXPAND | wxALL, 2);
    loadBox->Add(m_loadResourcesBtn, 0, wxEXPAND | wxALL, 2);
    loadBox->Add(m_loadActionsBtn, 0, wxEXPAND | wxALL, 2);
    
    // Botones de control
    wxStaticBoxSizer* btnBox = new wxStaticBoxSizer(wxVERTICAL, this, "Control");
    m_startBtn = new wxButton(this, 2004, "Iniciar Simulación");
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
    
    // Panel de información
    wxStaticBoxSizer* infoBox = new wxStaticBoxSizer(wxHORIZONTAL, this, "Información Cargada");
    
    // Lista de procesos
    wxStaticBoxSizer* processBox = new wxStaticBoxSizer(wxVERTICAL, this, "Procesos");
    m_processListCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(200, 120), 
                                       wxLC_REPORT | wxLC_SINGLE_SEL);
    m_processListCtrl->AppendColumn("PID", wxLIST_FORMAT_LEFT, 60);
    m_processListCtrl->AppendColumn("BT", wxLIST_FORMAT_RIGHT, 50);
    m_processListCtrl->AppendColumn("AT", wxLIST_FORMAT_RIGHT, 50);
    m_processListCtrl->AppendColumn("Priority", wxLIST_FORMAT_RIGHT, 60);
    processBox->Add(m_processListCtrl, 1, wxEXPAND | wxALL, 2);
    
    // Lista de recursos
    wxStaticBoxSizer* resourceBox = new wxStaticBoxSizer(wxVERTICAL, this, "Recursos");
    m_resourceListCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(200, 120), 
                                        wxLC_REPORT | wxLC_SINGLE_SEL);
    m_resourceListCtrl->AppendColumn("Recurso", wxLIST_FORMAT_LEFT, 100);
    m_resourceListCtrl->AppendColumn("Contador", wxLIST_FORMAT_RIGHT, 80);
    resourceBox->Add(m_resourceListCtrl, 1, wxEXPAND | wxALL, 2);
    
    // Lista de acciones
    wxStaticBoxSizer* actionBox = new wxStaticBoxSizer(wxVERTICAL, this, "Acciones");
    m_actionListCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(250, 120), 
                                      wxLC_REPORT | wxLC_SINGLE_SEL);
    m_actionListCtrl->AppendColumn("PID", wxLIST_FORMAT_LEFT, 50);
    m_actionListCtrl->AppendColumn("Acción", wxLIST_FORMAT_LEFT, 70);
    m_actionListCtrl->AppendColumn("Recurso", wxLIST_FORMAT_LEFT, 70);
    m_actionListCtrl->AppendColumn("Ciclo", wxLIST_FORMAT_RIGHT, 50);
    actionBox->Add(m_actionListCtrl, 1, wxEXPAND | wxALL, 2);
    
    infoBox->Add(processBox, 1, wxEXPAND | wxALL, 5);
    infoBox->Add(resourceBox, 1, wxEXPAND | wxALL, 5);
    infoBox->Add(actionBox, 1, wxEXPAND | wxALL, 5);
    
    // Timeline
    wxStaticBoxSizer* timelineBox = new wxStaticBoxSizer(wxVERTICAL, this, "Línea de Tiempo");
    m_timelineChart = new TimelineChart(this);
    timelineBox->Add(m_timelineChart, 1, wxEXPAND | wxALL, 2);
    
    // Layout principal
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(controlBox, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(infoBox, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(timelineBox, 1, wxEXPAND | wxALL, 5);
    
    SetSizer(mainSizer);
}
