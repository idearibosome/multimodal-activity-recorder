import numpy as np

def create_model(config):
  return SampleModel(config)

class SampleModel():

  DEFAULT_CONFIG = {
  }

  def __init__(self, config):
    # config
    self.config = config
    for (key, value) in self.DEFAULT_CONFIG.items():
      if not key in self.config:
        self.config[key] = value
  

  def prepare(self):
    pass
  
  def start(self):
    pass

  def recognize(self, data):
    return {'test': np.random.randint(5)}
  
  def stop(self):
    pass
  
  def finalize(self):
    pass
  
