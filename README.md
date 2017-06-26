# Asseto-Corsa-Glasses
Small C++ project to capture game video stream and game information of Asseto Corsa.

## Plan
End goal is making an AI to control cars in Asseto Corsa. Intermediate steps involve a way to effeciently 
pull visual information from the game and pipe it in to a neural network & recording software.

To do this I expect to create a DirectX 11 Hook, which I will inject myself, and on SwapChain grab the backbuffer.

I will then render it through a shader that allows for GPU resizing/greyscaling and other image processing.

This information will then be sent to the neural network.

- [ ] Create DLL Injection Library
- [ ] Create DirectX 11 Hook
- [ ] Create Shader and use Render Target to post-process the backbuffer.
- [ ] Pipe information out in a format that can be used by an AI system (Maybe tensorflow, undecided...)
- [ ] More... (My foresight isn't this good)
