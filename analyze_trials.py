"""
SafeStride AI - Data Visualization Script
Generates the kinematic failure matrix (Scatter Plot) from CSV trial data.
Dependencies: pandas, matplotlib
"""

import pandas as pd
import matplotlib.pyplot as plt

def generate_scatter_plot(csv_filepath):
    # Load the trial data
    # Expected columns: Trial_ID, Latency_ms, Speed_PWM, Stopping_Cushion_Inches, Status
    try:
        df = pd.read_csv(csv_filepath)
    except FileNotFoundError:
        print(f"Error: Could not find {csv_filepath}")
        return

    # Set up the plot aesthetics
    plt.figure(figsize=(10, 6))
    plt.style.use('dark_background') # Matches the presentation theme
    
    # Define colors based on trial outcome
    colors = {'Safe': '#10b981', 'Marginal': '#f59e0b', 'Crash': '#ef4444'}
    markers = {'Safe': '^', 'Marginal': 'o', 'Crash': 'x'}

    # Plot each category
    for status in colors.keys():
        subset = df[df['Status'] == status]
        plt.scatter(
            subset['Latency_ms'], 
            subset['Stopping_Cushion_Inches'],
            c=colors[status],
            marker=markers[status],
            label=status,
            s=100,
            alpha=0.8
        )

    # Draw the crucial Collision Line (0 inches)
    plt.axhline(0, color='#ef4444', linestyle='--', linewidth=2, label='Collision Boundary (0 in)')

    # Add labels and physics context
    plt.title('Kinematic Failure Matrix: Loop Latency vs. Stopping Cushion', fontsize=14, fontweight='bold')
    plt.xlabel('Execution Latency (ms)', fontsize=12)
    plt.ylabel('Stopping Cushion (Inches)', fontsize=12)
    
    plt.grid(True, linestyle=':', alpha=0.3)
    plt.legend(loc='upper right')
    
    # Save the output graph
    output_filename = 'safestride_results_plot.png'
    plt.savefig(output_filename, dpi=300, bbox_inches='tight')
    print(f"Success! Scatter plot saved as {output_filename}")

if __name__ == "__main__":
    # Point this to the CSV file you generate from your Arduino tests
    generate_scatter_plot('trials_data.csv')
