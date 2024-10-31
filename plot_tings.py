import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import matplotlib
from statsmodels.formula.api import ols
import statsmodels.api as sm
from statsmodels.stats.multicomp import pairwise_tukeyhsd
from scipy.stats import pearsonr
from sklearn.linear_model import LinearRegression
from scipy.stats import kruskal, friedmanchisquare, spearmanr, levene
import scikit_posthocs as sp
import numpy as np
from statsmodels.stats.multitest import multipletests


# Load the CSV file
df = pd.read_csv("./run_table (1).csv")

matplotlib.rcParams["axes.unicode_minus"] = False

# Calculate the total energy usage by summing the relevant energy columns
energy_columns = ["dram_energy", "package_energy", "pp0_energy", "pp1_energy"]
df["total_energy"] = df[energy_columns].sum(axis=1)

# Calculate the total CPU usage by summing the usage columns
cpu_columns = ["cpu_usage_0", "cpu_usage_1", "cpu_usage_2", "cpu_usage_3"]
df["total_cpu_usage"] = df[cpu_columns].sum(axis=1)

cpu_freq_columns = ["cpu_freq_0", "cpu_freq_1", "cpu_freq_2", "cpu_freq_3"]
df["total_cpu_freq"] = df[cpu_freq_columns].sum(axis=1)

plt.rcParams["figure.dpi"] = 300
plt.style.use(["vu.mplstyle", "twocolumn.mplstyle"])


# Function to generate barplots with error bars for total energy, CPU usage, and execution time
def plot_usage_barplot(
    df, usage_column, outer_group_by, inner_group_by, title_suffix, y_label, save_path
):
    """
    Generates a barplot with error bars of usage (energy, CPU, or execution time) grouped by an outer and inner grouping.

    Args:
    - df: DataFrame containing the dataset.
    - usage_column: The column representing the usage (e.g., total_energy, total_cpu_usage, or execution_time).
    - outer_group_by: The outer grouping column (e.g., 'algorithm').
    - inner_group_by: The inner grouping column (e.g., 'llm').
    - title_suffix: The suffix to add to the title (e.g., "Energy Usage", "CPU Usage", or "Execution Time").
    - save_path: Path where the barplot image will be saved.

    Returns:
    - Saves the barplot to the specified path.
    """
    # Combine the groupings into a new column for plotting
    df["group"] = df[[outer_group_by, inner_group_by]].agg(" - ".join, axis=1)

    # Create the barplot with error bars
    plt.figure(figsize=(16, 8))
    sns.barplot(
        x=outer_group_by,
        y=usage_column,
        hue=inner_group_by,
        data=df,
        ci="sd",  # Plot error bars based on standard deviation
        palette="Set2",
    )

    # Customize the plot
    plt.title(
        f"Total {title_suffix} ",
        fontsize=16,
    )
    plt.xlabel(outer_group_by.capitalize(), fontsize=14)
    plt.ylabel(f"Total {y_label}", fontsize=14)
    plt.xticks(rotation=45, fontsize=12)
    plt.yticks(fontsize=12)
    plt.legend(title=inner_group_by.capitalize(), title_fontsize="13", fontsize="11")
    plt.grid(True, linestyle="--", alpha=0.7)

    # Save the plot
    plt.tight_layout()
    plt.savefig(save_path)
    plt.show()


# Filter out rows where 'llm' is 'Gemeni' and 'algorithm' is 'BB'

df_filtered = df


plot = False
if plot:
    # Plot total energy usage
    plot_usage_barplot(
        df_filtered,
        "total_energy",
        "algorithm",
        "llm",
        "Total Energy Consumption by Algorithm and LLM",
        "Energy Usage (J)",
        "total_energy_barplot.png",
    )

    plot_usage_barplot(
        df_filtered,
        "total_cpu_freq",
        "algorithm",
        "llm",
        "CPU Frequency Distribution Across Algorithms and LLMs",
        "CPU Frequency (MHz)",
        "Frequency_barplot.png",
    )

    # Plot total CPU usage
    plot_usage_barplot(
        df_filtered,
        "total_cpu_usage",
        "algorithm",
        "llm",
        "CPU Utilization Across Algorithms and LLMs",
        "CPU Usage (%)",
        "total_cpu_usage_barplot.png",
    )

    # Plot execution time (assuming there's a column named 'execution_time')
    plot_usage_barplot(
        df_filtered,
        "run_time",  # Replace with the actual name of the column if different
        "algorithm",
        "llm",
        "Execution Time Comparison by Algorithm and LLM",
        "Execution Time (s)",
        "execution_time_barplot.png",
    )

    plot_usage_barplot(
        df_filtered,
        "used_memory",  # Replace with the actual name of the column if different
        "algorithm",
        "llm",
        "Memory Usage Across Algorithms and LLMs ",
        "Used memory (MB)",
        "used_memory.png",
    )

df_filtered = df[~((df["llm"] == "Gemeni") & (df["algorithm"] == "BB"))]
# Perform ANOVA on energy consumption by LLM
anova_model = ols("total_energy ~ C(llm)", data=df_filtered).fit()
anova_table = sm.stats.anova_lm(anova_model, typ=2)

# Display ANOVA results
print("ANOVA Results for Energy Consumption by LLM:")
print(anova_table)

# If ANOVA is significant (p-value < 0.05), perform Tukey's HSD test
if anova_table["PR(>F)"][0] < 0.05:
    tukey = pairwise_tukeyhsd(df_filtered["total_energy"], df_filtered["llm"])
    print("\nTukey's HSD Test Results:")
    print(tukey)
else:
    print(
        "\nNo statistically significant difference found in energy consumption between LLMs."
    )


# Correlation analysis between total energy and other components
correlation_columns = [
    "total_energy",
    "total_cpu_usage",
    "run_time",
    "used_memory",
    # "total_cpu_freq",
]

names = {
    "total_energy": "Energy Us.",
    "total_cpu_usage": "CPU Us.",
    "run_time": "Execution Time",
    "used_memory": "Memory",
    # "total_cpu_freq": "CPU Freq.",
}

# Apply the Kruskal-Wallis H test
stat, p_value = kruskal(
    df_filtered["total_energy"][df_filtered["llm"] == "ChatGPT"],
    df_filtered["total_energy"][df_filtered["llm"] == "Claude"],
    df_filtered["total_energy"][df_filtered["llm"] == "Gemeni"],
)

# Run Dunn's test for multiple comparisons
dunn_test_results = sp.posthoc_dunn(
    df, val_col="total_energy", group_col="llm", p_adjust="bonferroni"
)
print(dunn_test_results)

# Apply the Friedman test (assuming three sets of repeated measures)
stat, p_value = friedmanchisquare(
    df["total_energy"][df["llm"] == "ChatGPT"],
    df["total_energy"][df["llm"] == "Claude"],
    df["total_energy"][df["llm"] == "Gemeni"],
)

print(f"Friedman test statistic: {stat}, p-value: {p_value}")

print(f"Kruskal-Wallis H test statistic: {stat}, p-value: {p_value}")


correlation_matrix = df_filtered[correlation_columns].corr()

correlation_matrix.rename(index=names, columns=names, inplace=True)

# Display the correlation matrix
plt.figure(figsize=(8, 6))
sns.heatmap(correlation_matrix, annot=True, cmap="coolwarm", vmin=-1, vmax=1)
plt.title("Correlation Between Energy, CPU, and Memory Metrics")
plt.tight_layout()
plt.savefig("correlation_heatmap.png")
plt.show()


# Linear Regression Analysis to model the relationship between energy and CPU/Memory usage
X = df_filtered[["total_cpu_usage", "used_memory"]]
y = df_filtered["total_energy"]

# Fit the linear regression model
model = LinearRegression()
model.fit(X, y)

# Display regression coefficients
print(f"Intercept: {model.intercept_}")
print(f"Coefficients: {model.coef_}")

# Perform Pearson correlation between energy and CPU/memory usage
cpu_corr, cpu_pvalue = pearsonr(
    df_filtered["total_cpu_usage"], df_filtered["total_energy"]
)
memory_corr, memory_pvalue = pearsonr(
    df_filtered["used_memory"], df_filtered["total_energy"]
)

cpu_spearman_corr, cpu_spearman_pvalue = spearmanr(
    df_filtered["total_cpu_usage"], df_filtered["total_energy"]
)

import matplotlib.pyplot as plt
import seaborn as sns
import matplotlib.pyplot as plt
import seaborn as sns

# Plot 1: CPU Usage vs Energy Consumption with Regression Line
plt.figure(figsize=(6, 6))
sns.scatterplot(
    x="total_cpu_usage",
    y="total_energy",
    hue="algorithm",
    style="llm",
    data=df_filtered,
    palette="Set2",
)
sns.regplot(
    x="total_cpu_usage",
    y="total_energy",
    data=df_filtered,
    line_kws={"color": "red"},
    scatter=False,
)
plt.title("CPU Usage vs Energy Consumption")
plt.xlabel("Total CPU Usage (%)")
plt.ylabel("Total Energy (Joules)")
plt.tight_layout()
plt.savefig("cpu_usage_vs_energy_with_alg_llm.png")
plt.show()

# Plot 2: Memory Usage vs Energy Consumption with Regression Line
plt.figure(figsize=(6, 6))
sns.scatterplot(
    x="used_memory",
    y="total_energy",
    hue="algorithm",
    style="llm",
    data=df_filtered,
    palette="Set2",
)
sns.regplot(
    x="used_memory",
    y="total_energy",
    data=df_filtered,
    line_kws={"color": "red"},
    scatter=False,
)
plt.title("Memory Usage vs Energy Consumption")
plt.xlabel("Used Memory (MB)")
plt.ylabel("Total Energy (Joules)")
plt.tight_layout()
plt.savefig("memory_usage_vs_energy_with_alg_llm.png")
plt.show()

# Plot 3: Execution Time vs Energy Consumption with Regression Line
plt.figure(figsize=(6, 6))
sns.scatterplot(
    x="run_time",
    y="total_energy",
    hue="algorithm",
    style="llm",
    data=df_filtered,
    palette="Set2",
)
sns.regplot(
    x="run_time",
    y="total_energy",
    data=df_filtered,
    line_kws={"color": "red"},
    scatter=False,
)
plt.title("Execution Time vs Energy Consumption")
plt.xlabel("Execution Time (s)")
plt.ylabel("Total Energy (Joules)")
plt.tight_layout()
plt.savefig("execution_time_vs_energy_with_alg_llm.png")
plt.show()


levene_stat, levene_pvalue = levene(
    df_filtered["total_cpu_usage"], df_filtered["total_energy"]
)

print(f"Levene test statistic: {levene_stat}, p-value: {levene_pvalue}")
# Print Pearson correlation results
print(
    f"Pearson correlation (CPU Usage vs Energy): {cpu_corr:.6f}, p-value: {cpu_pvalue}"
)
print(
    f"Spearman correlation (CPU Usage vs Energy): {cpu_spearman_corr:.6f}, p-value: {cpu_spearman_pvalue}"
)
print(
    f"Pearson correlation (Memory Usage vs Energy): {memory_corr:.6f}, p-value: {memory_pvalue}"
)


print(
    f"Pearson correlation (CPU Usage vs Energy): {cpu_corr:.6f}, p-value: {cpu_pvalue}"
)
print(
    f"Pearson correlation (Memory Usage vs Energy): {memory_corr:.6f}, p-value: {memory_pvalue}"
)

# ===================
# Optional: Partial correlation analysis using linear regression
# You can use packages like `pingouin` to calculate partial correlation in Python
# Example energy consumption data for two LLMs (ChatGPT and Claude)
energy_chatgpt = df_filtered[df_filtered["llm"] == "ChatGPT"]["total_energy"]
energy_claude = df_filtered[df_filtered["llm"] == "Claude"]["total_energy"]

# Means and standard deviations
mean_chatgpt = np.mean(energy_chatgpt)
mean_claude = np.mean(energy_claude)
std_chatgpt = np.std(energy_chatgpt, ddof=1)  # sample standard deviation
std_claude = np.std(energy_claude, ddof=1)

# Sample sizes
n_chatgpt = len(energy_chatgpt)
n_claude = len(energy_claude)

# Pooled standard deviation
pooled_std = np.sqrt(
    ((n_chatgpt - 1) * std_chatgpt**2 + (n_claude - 1) * std_claude**2)
    / (n_chatgpt + n_claude - 2)
)

# Compute Cohen's d
cohen_d = (mean_chatgpt - mean_claude) / pooled_std
print(f"Cohen's d: {cohen_d}")

# Example energy consumption data for two LLMs (ChatGPT and Claude)
energy_chatgpt = df_filtered[df_filtered["llm"] == "ChatGPT"]["total_energy"]
energy_claude = df_filtered[df_filtered["llm"] == "Claude"]["total_energy"]

# Means and standard deviations
mean_chatgpt = np.mean(energy_chatgpt)
mean_claude = np.mean(energy_claude)
std_chatgpt = np.std(energy_chatgpt, ddof=1)  # sample standard deviation
std_claude = np.std(energy_claude, ddof=1)

# Sample sizes
n_chatgpt = len(energy_chatgpt)
n_claude = len(energy_claude)

# Pooled standard deviation
pooled_std = np.sqrt(
    ((n_chatgpt - 1) * std_chatgpt**2 + (n_claude - 1) * std_claude**2)
    / (n_chatgpt + n_claude - 2)
)

# Compute Cohen's d
cohen_d = (mean_chatgpt - mean_claude) / pooled_std
print(f"Cohen's d (ChatGPT vs Claude): {cohen_d:.4f}")

p_values = [0.000000000001, 0.000000000000001, 0.0000000000000001]

# Apply Bonferroni correction
_, corrected_pvalues, _, _ = multipletests(p_values, alpha=0.05, method="bonferroni")

# Print corrected p-values
for i, p in enumerate(corrected_pvalues):
    print(f"Corrected p-value for comparison {i+1}: {p:.4f}")
