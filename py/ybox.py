import hou

def currentPanetab():
	panename = hou.hscriptExpression('mousepane()').split('.')[-1] 
	return hou.ui.findPaneTab(panename)

def currentPanetabType():
	return type(currentPanetab())

def currentNode():
	return currentPanetab().currentNode()	

def currentCategory():
	if isinstance(currentPanetab(), hou.NetworkEditor):
		return currentPanetab().currentNode().type().category().name()
	return None


