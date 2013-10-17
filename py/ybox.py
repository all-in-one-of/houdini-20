import hou

def currentPanetab():
	panename = hou.hscriptExpression('mousepane()').split('.')[-1] 
	#print(panename)
	return hou.ui.findPaneTab(panename)

def currentPanetabType():
	#print(type(currentPanetab()))
	return type(currentPanetab())

def currentNode():
	#print(currentPanetab().currentNode())
	return currentPanetab().currentNode()	

def currentCategory():
	if isinstance(currentPanetab(), hou.NetworkEditor):
		#print(currentPanetab().currentNode().type().category())
		return currentPanetab().currentNode().type().category()
	return None

def gotoCategory(goto_category):
	pane = currentPanetab()
	goto_path = hou.expandString("$LAST{0}".format(goto_category.upper()))
	current_path = currentNode().path()
	current_category = current_path.split('/')[1]

	if goto_category == current_category:
		return False

	# save our lastnode in this context
	hou.hscript('set -g LAST{0} = {1}'.format(current_category.upper(), current_path))
	# then goto destination context
	goto_root =  (not goto_path) or (goto_path.count('/')==1)
	if goto_root:
		pane.cd("/{0}".format(goto_category))
	elif goto_path:
		pane.setCurrentNode(hou.node(goto_path))

	print("\nset $LAST{0} ({1})".format(current_category.upper(), current_path))
	print("goto $LAST{0} ({1})".format(goto_category.upper(), goto_path))
	return True
