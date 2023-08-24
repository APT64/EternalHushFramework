class AttributeNotFound(Exception):
    def __init__(self, *args):
        self.msg = args[0]
      
    def __str__(self):
        return "Attribute {0} does not exist".format(self.msg)
        
