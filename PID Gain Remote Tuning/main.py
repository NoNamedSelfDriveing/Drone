from PyQt4 import QtGui
from window import Window

if __name__ == '__main__':
    def run():
        app = QtGui.QApplication([])
        GUI = Window()
        app.exec_()

    run()

