import matplotlib.pyplot as plt
import numpy as np

# Pie chart, where the slices will be ordered and plotted counter-clockwise:
labels = 'Slippage Loss', 'Transaction Fees', 'Gas Fees'
data = [181998.32, 526105.77, 115220]

fig1, ax1 = plt.subplots()

colors = ['#c746c5','#51b5d6','#e0e0e0']

def func(pct, allvalues):
    absolute = float(pct / 100.*np.sum(allvalues))
    return "{:.1f}%\n(${:.2f})".format(pct, absolute)

ax1.pie(data, labels=labels, autopct = lambda pct: func(pct, data),
        shadow=True, startangle=90, colors = colors)
ax1.axis('equal')  # Equal aspect ratio ensures that pie is drawn as a circle.
plt.title("SushiSwap Daily Fee Breakdown (top 10 tokens by volume)")
plt.show()