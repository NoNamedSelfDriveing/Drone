from PyQt4 import QtGui, QtCore
import USBSerial


class Window(QtGui.QMainWindow):
    lblGain = []    # Label for show 'P', 'I', 'D'
    lblAxis = []    # Label for show 'Roll', 'Pitch', 'Yaw'
    txtRollGain = []    # TextBox for Roll Axis Gain P, I, D
    txtPitchGain = []   # TextBox for Pitch Axis Gain P, I, D
    txtYawGain = []     # TextBox for Yaw Axis Gain P, I, D
    chkGain = []        # CheckBox for Enable/Disable Changing Roll, Pitch, Yaw Gain
    lblTitle = None     # Label for show "Tuning Gain Value"
    cmbPort = None      # ComboBox for select COM Port
    btnSendGain = None  # Button for Transmit Gain Data
    enableSerial = [] # Enable Serial Port
    msg = None          # Message Box

    def __init__(self):
        super(Window, self).__init__()
        self.setGeometry(1200, 200, 650, 650)
        self.setWindowTitle("Remote Gain Tuning")
        self.setWindowIcon(QtGui.QIcon('icon.jpg'))
        self.home()

    def home(self):
        self.createInstance()
        self.showEnableCOMPort()
        self.show()

    # Create Instance(TextBox, Label, etc)
    def createInstance(self):
        # Label for show "Tuning Gain Value"
        self.lblTitle = QtGui.QLabel("Tuning Gain Value", self)
        self.lblTitle.setFont(QtGui.QFont('SansSerif', 25, QtGui.QFont.Bold))
        self.lblTitle.resize(self.lblTitle.sizeHint())
        self.lblTitle.move(30, 55)

        # Label for show "Baud Rate"
        self.lblBaudRate = QtGui.QLabel("Baud Rate", self)
        self.lblBaudRate.setFont(QtGui.QFont('SansSerif', 16, QtGui.QFont.Bold))
        self.lblBaudRate.resize(self.lblTitle.sizeHint())
        self.lblBaudRate.move(360, 32)

        # Label for show "Port"
        self.lblBaudRate = QtGui.QLabel("Port", self)
        self.lblBaudRate.setFont(QtGui.QFont('SansSerif', 16, QtGui.QFont.Bold))
        self.lblBaudRate.resize(self.lblTitle.sizeHint())
        self.lblBaudRate.move(540, 32)

        # ComboBox for select Baudrate
        self.cmbBaudrate = QtGui.QComboBox(self)
        self.cmbBaudrate.setFont(QtGui.QFont('SansSerif', 15))
        self.cmbBaudrate.resize(120, 50)
        self.cmbBaudrate.move(350, 70)
        self.cmbBaudrate.addItem('9600')
        self.cmbBaudrate.addItem('19200')
        self.cmbBaudrate.addItem('38400')
        self.cmbBaudrate.addItem('57600')
        self.cmbBaudrate.addItem('115200')

        # ComboBox for select COM Port
        self.cmbPort = QtGui.QComboBox(self)
        self.cmbPort.setFont(QtGui.QFont('SansSerif', 15))
        self.cmbPort.resize(120, 50)
        self.cmbPort.move(500, 70)

        # Button for Transmit Gain Data
        self.btnSendGain = QtGui.QPushButton('TRANSMIT', self)
        self.btnSendGain.clicked.connect(self.transmitGainValue)
        self.btnSendGain.setFont(QtGui.QFont('DIN', 20))
        self.btnSendGain.resize(200, 100)
        self.btnSendGain.move(250, 530)

        # Message Box for noticing information
        self.msg = QtGui.QMessageBox()
        self.msg.setIcon(QtGui.QMessageBox.Information)
        self.msg.setText('Transmit Complete')
        self.msg.setWindowTitle('Notice')
        self.msg.setStandardButtons(QtGui.QMessageBox.Ok)
        self.msg.setGeometry(1450, 500, 650, 650)

        for i in range(0, 3):
            label = ['P', 'I', 'D']
            lblGainObj = QtGui.QLabel(label[i], self)
            lblGainObj.setFont(QtGui.QFont('SansSerif', 20))
            lblGainObj.resize(lblGainObj.sizeHint())
            lblGainObj.move(240 + 150 * i, 140)
            self.lblGain.append(lblGainObj)

        for i in range(0, 3):
            label = ['Roll', 'Pitch', 'Yaw']
            lblAxisObj = QtGui.QLabel(label[i], self)
            lblAxisObj.setFont(QtGui.QFont('SansSerif', 30, QtGui.QFont.Bold))
            lblAxisObj.resize(lblAxisObj.sizeHint())
            lblAxisObj.move(50, 190 + 120 * i)
            self.lblAxis.append(lblAxisObj)

        for i in range(0, 3):
            txtRollGainObj = QtGui.QLineEdit(self)
            txtRollGainObj.setFont(QtGui.QFont('Times', 14))
            txtRollGainObj.resize(100, 40)
            txtRollGainObj.move(200 + 150 * i, 200)
            self.txtRollGain.append(txtRollGainObj)

        for i in range(0, 3):
            txtPitchGainObj = QtGui.QLineEdit(self)
            txtPitchGainObj.setFont(QtGui.QFont('Times', 14))
            txtPitchGainObj.resize(100, 40)
            txtPitchGainObj.move(200 + 150 * i, 320)
            self.txtPitchGain.append(txtPitchGainObj)

        for i in range(0, 3):
            txtYawGainObj = QtGui.QLineEdit(self)
            txtYawGainObj.setFont(QtGui.QFont('Times', 14))
            txtYawGainObj.resize(100, 40)
            txtYawGainObj.move(200 + 150 * i, 440)
            self.txtYawGain.append(txtYawGainObj)

        for i in range(0, 3):
            chkGainObj = QtGui.QCheckBox(self)
            chkGainObj.stateChanged.connect(self.selectToChangeGain)
            chkGainObj.move(20, 160 + 125 * i)
            chkGainObj.resize(100, 100)
            self.chkGain.append(chkGainObj)

    # CheckBox Event Handler
    def selectToChangeGain(self, state):
        ROLL = 0
        PITCH = 1
        YAW = 2

        if state == QtCore.Qt.Checked:
            if self.chkGain[ROLL].isChecked():
                for textBox in self.txtRollGain:
                    textBox.setEnabled(False)
            if self.chkGain[PITCH].isChecked():
                for textBox in self.txtPitchGain:
                    textBox.setEnabled(False)
            if self.chkGain[YAW].isChecked():
                for textBox in self.txtYawGain:
                    textBox.setEnabled(False)

        else:
            if not self.chkGain[ROLL].isChecked():
                for textBox in self.txtRollGain:
                    textBox.setEnabled(True)
            if not self.chkGain[PITCH].isChecked():
                for textBox in self.txtPitchGain:
                    textBox.setEnabled(True)
            if not self.chkGain[YAW].isChecked():
                for textBox in self.txtYawGain:
                    textBox.setEnabled(True)

    # show Enable Communication COM Port on ComboBox
    def showEnableCOMPort(self):
        self.enableSerial = USBSerial.findEnableCOMPort()
        for ser in self.enableSerial:
            self.cmbPort.addItem(ser)

    # TRANSMIT Button Event Handler
    def transmitGainValue(self):
        txtGain = [self.txtRollGain, self.txtPitchGain, self.txtYawGain]
        gainType = [0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49]
        gainTypeIndex = 0
        baudRate = None

        # find now selected serial port
        for port in self.enableSerial:
            if port == self.cmbPort.currentText():
                selectedSerial = port
                break

        # find now selected baud rate
        baudRate = int(self.cmbBaudrate.currentText())

        print(selectedSerial)

        # if TextBox is not empty, transmit gain value of the TextBox
        for gain in txtGain:
            # Iterate Gain Term First
            for gainTermIndex in range(0, 3):
                if gain[gainTermIndex].text() != '' and gain[gainTermIndex].isEnabled():
                    USBSerial.transmitPacket(selectedSerial, baudRate, gainType[gainTypeIndex], float(gain[gainTermIndex].text()))
                gainTypeIndex += 1

        # Show Message Box for notice transmit complete
        self.msg.show()