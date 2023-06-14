
const get_index_array_sorted_by_values = (vals: number[]): number[] => {
    const idx_val = vals.map((v: number, i: number) => ({ v, i }));
    idx_val.sort((a, b) => a.v - b.v);

    return idx_val.map((obj) => obj.i);
};

export { get_index_array_sorted_by_values };