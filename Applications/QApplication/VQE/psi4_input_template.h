/*
Copyright (c) 2017-2018 Origin Quantum Computing. All Right Reserved.
Licensed under the Apache License 2.0

psi4_input_template.h

Author: LiYe
Created in 2018-12-13


*/
#ifndef PSI4_INPUT_TEMPLATE_H
#define PSI4_INPUT_TEMPLATE_H

namespace QPanda
{
    const char kPsi4_s [] =
        "import copy                                                                      \n"
        "import itertools                                                                 \n"
        "import numpy                                                                     \n"
        "import traceback                                                                 \n"
        "                                                                                 \n"
        "def general_basis_change(general_tensor, rotation_matrix, key):                  \n"
        "    \"\"\"Change the basis of an general interaction tensor.                     \n"
        "                                                                                 \n"
        "    M'^{p_1p_2...p_n} = R^{p_1}_{a_1} R^{p_2}_{a_2} ...                          \n"
        "                        R^{p_n}_{a_n} M^{a_1a_2...a_n} R^{p_n}_{a_n}^T ...       \n"
        "                        R^{p_2}_{a_2}^T R_{p_1}_{a_1}^T                          \n"
        "                                                                                 \n"
        "    where R is the rotation matrix, M is the general tensor, M' is the           \n"
        "    transformed general tensor, and a_k and p_k are indices. The formula uses    \n"
        "    the Einstein notation (implicit sum over repeated indices).                  \n"
        "                                                                                 \n"
        "    In case R is complex, the k-th R in the above formula need to be conjugated  \n"
        "    if key has a 1 in the k-th place (meaning that the corresponding operator    \n"
        "    is a creation operator).                                                     \n"
        "                                                                                 \n"
        "    Args:                                                                        \n"
        "        general_tensor: A square numpy array or matrix containing information    \n"
        "            about a general interaction tensor.                                  \n"
        "        rotation_matrix: A square numpy array or matrix having dimensions of     \n"
        "            n_qubits by n_qubits. Assumed to be unitary.                         \n"
        "        key: A tuple indicating the type of general_tensor. Assumed to be        \n"
        "            non-empty. For example, a tensor storing coefficients of             \n"
        "            :math:`a^\\dagger_p a_q` would have a key of (1, 0) whereas a tensor \n"
        "            storing coefficients of :math:`a^\\dagger_p a_q a_r a^\\dagger_s`     \n"
        "            would have a key of (1, 0, 0, 1).                                    \n"
        "                                                                                 \n"
        "    Returns:                                                                     \n"
        "        transformed_general_tensor: general_tensor in the rotated basis.         \n"
        "    \"\"\"                                                                       \n"
        "    # If operator acts on spin degrees of freedom, enlarge rotation matrix.      \n"
        "    n_orbitals = rotation_matrix.shape[0]                                        \n"
        "    if general_tensor.shape[0] == 2 * n_orbitals:                                \n"
        "        rotation_matrix = numpy.kron(rotation_matrix, numpy.eye(2))              \n"
        "                                                                                 \n"
        "    order = len(key)                                                             \n"
        "    if order > 26:                                                               \n"
        "        raise ValueError('Order exceeds maximum order supported (26).')          \n"
        "                                                                                 \n"
        "    # Do the basis change through a single call of numpy.einsum. For example,    \n"
        "    # for the (1, 1, 0, 0) tensor, the call is:                                  \n"
        "    #     numpy.einsum('abcd,aA,bB,cC,dD',                                       \n"
        "    #                  general_tensor,                                           \n"
        "    #                  rotation_matrix.conj(),                                   \n"
        "    #                  rotation_matrix.conj(),                                   \n"
        "    #                  rotation_matrix,                                          \n"
        "    #                  rotation_matrix)                                          \n"
        "                                                                                 \n"
        "    # The 'abcd' part of the subscripts                                          \n"
        "    subscripts_first = ''.join(chr(ord('a') + i) for i in range(order))          \n"
        "                                                                                 \n"
        "    # The 'Aa,Bb,Cc,Dd' part of the subscripts                                   \n"
        "    subscripts_rest = ','.join(chr(ord('a') + i) +                               \n"
        "                               chr(ord('A') + i) for i in range(order))          \n"
        "                                                                                 \n"
        "    subscripts = subscripts_first + ',' + subscripts_rest                        \n"
        "                                                                                 \n"
        "    # The list of rotation matrices, conjugated as necessary.                    \n"
        "    rotation_matrices = [rotation_matrix.conj() if x else                        \n"
        "                         rotation_matrix for x in key]                           \n"
        "                                                                                 \n"
        "    # \"optimize = True\" does greedy optimization, which will be enough here.   \n"
        "    transformed_general_tensor = numpy.einsum(subscripts,                        \n"
        "                                              general_tensor,                    \n"
        "                                              *rotation_matrices,                \n"
        "                                              optimize=True)                     \n"
        "    return transformed_general_tensor                                            \n"
        "                                                                                 \n"
        "class MyPolynomialTensor(object):                                                \n"
        "    def __init__(self, n_body_tensors):                                          \n"
        "        self.n_body_tensors = n_body_tensors                                     \n"
        "        key_iterator = iter(n_body_tensors.keys())                               \n"
        "        key = next(key_iterator)                                                 \n"
        "        if key == ():                                                            \n"
        "            key = next(key_iterator)                                             \n"
        "        self.n_qubits = n_body_tensors[key].shape[0]                             \n"
        "                                                                                 \n"
        "    def __getitem__(self, args):                                                 \n"
        "        \"\"\"Look up matrix element.                                            \n"
        "                                                                                 \n"
        "        Args:                                                                    \n"
        "            args: Tuples indicating which coefficient to get. For instance,      \n"
        "                `my_tensor[(6, 1), (8, 1), (2, 0)]`                              \n"
        "                returns                                                          \n"
        "                `my_tensor.n_body_tensors[1, 1, 0][6, 8, 2]`                     \n"
        "        \"\"\"                                                                   \n"
        "        if len(args) == 0:                                                       \n"
        "            return self.n_body_tensors[()]                                       \n"
        "        else:                                                                    \n"
        "            index = tuple([operator[0] for operator in args])                    \n"
        "            key = tuple([operator[1] for operator in args])                      \n"
        "            return self.n_body_tensors[key][index]                               \n"
        "                                                                                 \n"
        "    def __iter__(self):                                                          \n"
        "        \"\"\"Iterate over non-zero elements of PolynomialTensor.\"\"\"          \n"
        "        def sort_key(key):                                                       \n"
        "            \"\"\"This determines how the keys to n_body_tensors                 \n"
        "            should be sorted.\"\"\"                                              \n"
        "            # Interpret key as an integer written in binary                      \n"
        "            if key == ():                                                        \n"
        "                return 0, 0                                                      \n"
        "            else:                                                                \n"
        "                key_int = int(''.join(map(str, key)))                            \n"
        "                return len(key), key_int                                         \n"
        "                                                                                 \n"
        "        for key in sorted(self.n_body_tensors.keys(), key=sort_key):             \n"
        "            if key == ():                                                        \n"
        "                yield ()                                                         \n"
        "            else:                                                                \n"
        "                n_body_tensor = self.n_body_tensors[key]                         \n"
        "                for index in itertools.product(                                  \n"
        "                        range(self.n_qubits), repeat=len(key)):                  \n"
        "                    if n_body_tensor[index]:                                     \n"
        "                        yield tuple(zip(index, key))                             \n"
        "                                                                                 \n"
        "    def __str__(self):                                                           \n"
        "        \"\"\"Print out the non-zero elements of PolynomialTensor.\"\"\"         \n"
        "        strings = []                                                             \n"
        "        for key in self:                                                         \n"
        "            strings.append('{} : {}\\n'.format(key, self[key]))                  \n"
        "        return ''.join(strings) if strings else '0'                              \n"
        "                                                                                 \n"
        "try:                                                                             \n";
    const char kMolecular_s [] =
        "    # Set molecular geometry and symmetry.                                           \n"
        "    molecule mol {                                                                   \n";
    const char kMolecular_e [] =
        "    symmetry c1                                                                      \n"
        "    }                                                                                \n";
    const char kMultiplicit_s [] = "    mol.set_multiplicity(";
    const char kMultiplicit_e [] = ")\n";
    const char kCharge_s [] = "    mol.set_molecular_charge(";
    const char kCharge_e [] = ")\n";

    const char kGlobals_s [] =
        "    # Set global parameters of calculation.                                          \n"
        "    set globals {                                                                    \n";
    const char kBasis [] = "    basis ";
    const char kGlobals_e [] =
        "\n        scf_type pk                                                                \n"
        "        soscf false                                                                  \n"
        "        freeze_core false                                                            \n"
        "        df_scf_guess false                                                           \n"
        "        opdm true                                                                    \n"
        "        tpdm true                                                                    \n"
        "        maxiter 1e6                                                                  \n"
        "        num_amps_print 1e6                                                           \n"
        "        r_convergence 1e-6                                                           \n"
        "        d_convergence 1e-6                                                           \n"
        "        e_convergence 1e-6                                                           \n"
        "        damping_percentage 0                                                         \n"
        "    }                                                                                \n";
    const char kPsi4_e [] =
        "    if mol.multiplicity == 1:                                                        \n"
        "        set reference rhf                                                            \n"
        "        set guess sad                                                                \n"
        "    else:                                                                            \n"
        "        set reference rohf                                                           \n"
        "        set guess gwh                                                                \n"
        "    # Run self-consistent field (SCF) calculation.                                   \n"
        "    hf_energy, hf_wavefunction = energy('scf', return_wfn=True)                  \n"
        "except Exception as exception:                                                   \n"
        "    fo = open(\"psi4.log\", \"w\")                                               \n"
        "    fo.write(traceback.format_exc())                                             \n"
        "    fo.close()                                                                   \n"
        "    raise                                                                        \n"
        "else:                                                                            \n"
        "    # Get orbitals and Fock matrix.                                              \n"
        "    nuclear_repulsion = mol.nuclear_repulsion_energy()                           \n"
        "    canonical_orbitals = numpy.asarray(hf_wavefunction.Ca())                     \n"
        "    n_orbitals = canonical_orbitals.shape[0]                                     \n"
        "    n_qubits = 2 * n_orbitals                                                    \n"
        "                                                                                 \n"
        "    # Get integrals using MintsHelper.                                           \n"
        "    mints = MintsHelper(hf_wavefunction.basisset())                              \n"
        "    one_body_integrals = general_basis_change(                                   \n"
        "        numpy.asarray(mints.ao_kinetic()), canonical_orbitals, (1, 0))           \n"
        "    one_body_integrals += general_basis_change(                                  \n"
        "        numpy.asarray(mints.ao_potential()), canonical_orbitals, (1, 0))         \n"
        "    two_body_integrals = numpy.asarray(mints.ao_eri())                           \n"
        "    two_body_integrals.reshape((n_orbitals, n_orbitals,                          \n"
        "                                n_orbitals, n_orbitals))                         \n"
        "    two_body_integrals = numpy.einsum('psqr', two_body_integrals)                \n"
        "    two_body_integrals = general_basis_change(                                   \n"
        "        two_body_integrals, canonical_orbitals, (1, 1, 0, 0))                    \n"
        "                                                                                 \n"
        "n_qubits = 2 * one_body_integrals.shape[0]                                       \n"
        "                                                                                 \n"
        "# Initialize Hamiltonian coefficients.                                           \n"
        "one_body_coefficients = numpy.zeros((n_qubits, n_qubits))                        \n"
        "two_body_coefficients = numpy.zeros((n_qubits, n_qubits,                         \n"
        "                                     n_qubits, n_qubits))                        \n"
        "# Loop through integrals.                                                        \n"
        "for p in range(n_qubits // 2):                                                   \n"
        "    for q in range(n_qubits // 2):                                               \n"
        "                                                                                 \n"
        "        # Populate 1-body coefficients. Require p and q have same spin.          \n"
        "        one_body_coefficients[2 * p, 2 * q] = one_body_integrals[                \n"
        "            p, q]                                                                \n"
        "        one_body_coefficients[2 * p + 1, 2 *                                     \n"
        "                              q + 1] = one_body_integrals[p, q]                  \n"
        "        # Continue looping to prepare 2-body coefficients.                       \n"
        "        for r in range(n_qubits // 2):                                           \n"
        "            for s in range(n_qubits // 2):                                       \n"
        "                                                                                 \n"
        "                # Mixed spin                                                     \n"
        "                two_body_coefficients[2 * p, 2 * q + 1,                          \n"
        "                                      2 * r + 1, 2 * s] = (                      \n"
        "                    two_body_integrals[p, q, r, s] / 2.)                         \n"
        "                two_body_coefficients[2 * p + 1, 2 * q,                          \n"
        "                                      2 * r, 2 * s + 1] = (                      \n"
        "                    two_body_integrals[p, q, r, s] / 2.)                         \n"
        "                                                                                 \n"
        "                # Same spin                                                      \n"
        "                two_body_coefficients[2 * p, 2 * q,                              \n"
        "                                      2 * r, 2 * s] = (                          \n"
        "                    two_body_integrals[p, q, r, s] / 2.)                         \n"
        "                two_body_coefficients[2 * p + 1, 2 * q + 1,                      \n"
        "                                      2 * r + 1, 2 * s + 1] = (                  \n"
        "                    two_body_integrals[p, q, r, s] / 2.)                         \n"
        "                                                                                 \n"
        "EQ_TOLERANCE = 1e-8                                                              \n"
        "# Truncate.                                                                      \n"
        "one_body_coefficients[                                                           \n"
        "    numpy.absolute(one_body_coefficients) < EQ_TOLERANCE] = 0.                   \n"
        "two_body_coefficients[                                                           \n"
        "    numpy.absolute(two_body_coefficients) < EQ_TOLERANCE] = 0.                   \n"
        "                                                                                 \n"
        "                                                                                 \n"
        "operator = MyPolynomialTensor(                                                   \n"
        "             {(): nuclear_repulsion,                                             \n"
        "             (1, 0): one_body_coefficients,                                      \n"
        "             (1, 1, 0, 0): two_body_coefficients})                               \n"
        "                                                                                 \n"
        "fo = open(\"psi4.data.tmp\", \"w\")                                              \n"
        "fo.write(operator.__str__())                                                     \n"
        "fo.close()                                                                       \n";
}

#endif // PSI4_INPUT_TEMPLATE_H