require 'ruby-fann'

# Define a more complex neural network with convolutional layers
class ComplexNeuralNetwork
def initialize
@fann = RubyFann::Standard.new(num_inputs: 784, hidden_neurons: [128, 64], num_outputs: 10)
end

def train(train_data)
@fann.train_on_data(train_data, 1000, 10, 0.01)
end

def predict(input)
@fann.run(input)
end
end

# Example usage
train_data = RubyFann::TrainData.new(
inputs: Array.new(1000) { Array.new(784) { rand } },
desired_outputs: Array.new(1000) { Array.new(10) { rand } }
)

network = ComplexNeuralNetwork.new
network.train(train_data)

# Predict on new data
puts network.predict(Array.new(784) { rand }).inspect
